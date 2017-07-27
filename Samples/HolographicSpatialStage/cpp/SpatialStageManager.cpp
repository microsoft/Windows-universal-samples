#include "pch.h"
#include "SpatialStageManager.h"

using namespace HolographicSpatialStage;

using namespace DirectX;
using namespace Platform;
using namespace std::placeholders;
using namespace Windows::Foundation;
using namespace Windows::Perception::Spatial;
using namespace Windows::Foundation::Numerics;

SpatialStageManager::SpatialStageManager(
    const std::shared_ptr<DX::DeviceResources>& deviceResources, 
    const std::shared_ptr<SceneController>& sceneController)
    : m_deviceResources(deviceResources), m_sceneController(sceneController)
{
    // Get notified when the stage is updated.
    m_spatialStageChangedEventToken = SpatialStageFrameOfReference::CurrentChanged +=
        ref new EventHandler<Object^>(std::bind(&SpatialStageManager::OnCurrentChanged, this, _1));

    // Make sure to get the current spatial stage.
    OnCurrentChanged(nullptr);
}

SpatialStageManager::~SpatialStageManager()
{
    SpatialStageFrameOfReference::CurrentChanged -= m_spatialStageChangedEventToken;
}

void SpatialStageManager::OnCurrentChanged(Object^ /*o*/)
{
    // The event notifies us that a new stage is available.
    // Get the current stage.
    m_currentStage = SpatialStageFrameOfReference::Current;
    m_currentCoordinateSystem = nullptr;
    m_stationaryFrameOfReference = nullptr;

    // Clear previous content.
    m_sceneController->ClearSceneObjects();

    XMFLOAT3 visibleAreaColor;
    std::vector<float3> visibleAreaIndicatorVertices;

    if (m_currentStage != nullptr)
    {
        // Obtain stage geometry.
        m_currentCoordinateSystem = m_currentStage->CoordinateSystem;
        auto boundsVertexArray = m_currentStage->TryGetMovementBounds(m_currentCoordinateSystem);
        if (boundsVertexArray->Length > 0)
        {
            // Visualize the area where the user can move around.
            std::vector<float3> boundsVertices;
            boundsVertices.resize(boundsVertexArray->Length);
            memcpy(boundsVertices.data(), boundsVertexArray->Data, boundsVertexArray->Length * sizeof(float3));
            std::vector<unsigned short> indices = TriangulatePoints(boundsVertices);
            m_stageBoundsShape =
                std::make_shared<SceneObject>(
                    m_deviceResources,
                    reinterpret_cast<std::vector<XMFLOAT3>&>(boundsVertices),
                    indices,
                    XMFLOAT3(DirectX::Colors::SeaGreen),
                    m_currentCoordinateSystem);
            m_sceneController->AddSceneObject(m_stageBoundsShape);
        }

        // In this sample, we draw a visual indicator for some spatial stage properties.
        // If the view is forward-only, the indicator is a half circle pointing forward - otherwise, it
        // is a full circle.
        // If the user can walk around, the indicator is blue. If the user is seated, it is red.

        // The indicator is rendered at the origin - which is where the user declared the center of the
        // stage to be during setup - above the plane of the stage bounds object.
        float3 visibleAreaCenter = float3(0.f, 0.001f, 0.f);

        // Its shape depends on the look direction range.
        if (m_currentStage->LookDirectionRange == SpatialLookDirectionRange::ForwardOnly)
        {
            // Half circle for forward-only look direction range.
            visibleAreaIndicatorVertices = CreateCircle(visibleAreaCenter, 0.25f, 9, XM_PI);
        }
        else
        {
            // Full circle for omnidirectional look direction range.
            visibleAreaIndicatorVertices = CreateCircle(visibleAreaCenter, 0.25f, 16, XM_2PI);
        }

        // Its color depends on the movement range.
        if (m_currentStage->MovementRange == SpatialMovementRange::NoMovement)
        {
            visibleAreaColor = XMFLOAT3(DirectX::Colors::OrangeRed);
        }
        else
        {
            visibleAreaColor = XMFLOAT3(DirectX::Colors::Aqua);
        }
    }
    else
    {
        // No spatial stage was found.
        // Fall back to a stationary coordinate system.
        auto locator = SpatialLocator::GetDefault();
        if (locator)
        {
            m_stationaryFrameOfReference = locator->CreateStationaryFrameOfReferenceAtCurrentLocation(float3(0.f, -0.5f, 0.f));
            if (m_stationaryFrameOfReference)
            {
                m_currentCoordinateSystem = m_stationaryFrameOfReference->CoordinateSystem;

                // Render an indicator, so that we know we fell back to a mode without a stage.
                visibleAreaIndicatorVertices = CreateCircle(float3(0.f), 0.125f, 16, XM_2PI);
                visibleAreaColor = XMFLOAT3(DirectX::Colors::LightSlateGray);
            }
        }
    }

    if (m_currentCoordinateSystem != nullptr)
    {
        std::vector<unsigned short> visibleAreaIndicatorIndices = TriangulatePoints(visibleAreaIndicatorVertices);
        m_stageVisibleAreaIndicatorShape =
            std::make_shared<SceneObject>(
                m_deviceResources,
                reinterpret_cast<std::vector<XMFLOAT3>&>(visibleAreaIndicatorVertices),
                visibleAreaIndicatorIndices,
                visibleAreaColor,
                m_currentCoordinateSystem);
        m_sceneController->AddSceneObject(m_stageVisibleAreaIndicatorShape);
    }
}

SpatialCoordinateSystem^ SpatialStageManager::GetCoordinateSystemForCurrentStage()
{ 
    return m_currentCoordinateSystem;
}

// Creates a 2D circle in the x-z plane, with the specified properties.
std::vector<float3> SpatialStageManager::CreateCircle(float3 center, float radius, int divisions, float radians)
{
    std::vector<float3> vertices(divisions);

    for (int i = 0; i < divisions; ++i)
    {
        float radiansThisIteration = radians * float(i) / float(divisions);
        vertices[i] =
            {
                center.x + (radius * -cos(radiansThisIteration)), 
                center.y,
                center.z + (radius * -sin(radiansThisIteration))
            };
    }

    return vertices;
}

// Triangulates a 2D shape, such as the spatial stage movement bounds.
// This function expects a set of vertices that define the boundaries of a shape, in
// clockwise order.
std::vector<unsigned short> SpatialStageManager::TriangulatePoints(std::vector<float3> const& vertices)
{
    size_t const& vertexCount = vertices.size();

    // Segments of the shape are removed as they are triangularized.
    std::vector<bool> vertexRemoved;
    vertexRemoved.resize(vertexCount, false);
    unsigned int vertexRemovedCount = 0;

    // Indices are used to define triangles.
    std::vector<unsigned short> indices;

    // Decompose into convex segments.
    unsigned short currentVertex = 0;
    while (vertexRemovedCount < (vertexCount - 2))
    {
        // Get next triangle:
        // Start with the current vertex.
        unsigned short index1 = currentVertex;

        // Get the next available vertex.
        unsigned short index2 = index1 + 1;

        // This cycles to the next available index.
        auto CycleIndex = [=](unsigned short indexToCycle, unsigned short stopIndex)
        {
            // Make sure the index does not exceed bounds.
            if (indexToCycle >= unsigned short(vertexCount))
            {
                indexToCycle -= unsigned short(vertexCount);
            }

            while (vertexRemoved[indexToCycle])
            {
                // If the vertex is removed, go to the next available one.
                ++indexToCycle;

                // Make sure the index does not exceed bounds.
                if (indexToCycle >= unsigned short(vertexCount))
                {
                    indexToCycle -= unsigned short(vertexCount);
                }

                // Prevent cycling all the way around.
                // Should not be needed, as we limit with the vertex count.
                if (indexToCycle == stopIndex)
                {
                    break;
                }
            }

            return indexToCycle;
        };
        index2 = CycleIndex(index2, index1);

        // Get the next available vertex after that.
        unsigned short index3 = index2 + 1;
        index3 = CycleIndex(index3, index1);

        // Vertices that may define a triangle inside the 2D shape.
        auto& v1 = vertices[index1];
        auto& v2 = vertices[index2];
        auto& v3 = vertices[index3];

        // If the projection of the first segment (in clockwise order) onto the second segment is 
        // positive, we know that the clockwise angle is less than 180 degrees, which tells us 
        // that the triangle formed by the two segments is contained within the bounding shape.
        auto v2ToV1 = v1 - v2;
        auto v2ToV3 = v3 - v2;
        float3 normalToV2ToV3 = { -v2ToV3.z, 0.f, v2ToV3.x };
        float projectionOntoNormal = dot(v2ToV1, normalToV2ToV3);
        if (projectionOntoNormal >= 0)
        {
            // Triangle is contained within the 2D shape.

            // Remove peak vertex from the list.
            vertexRemoved[index2] = true;
            ++vertexRemovedCount;

            // Create the triangle.
            indices.push_back(index1);
            indices.push_back(index2);
            indices.push_back(index3);

            // Continue on to the next outer triangle.
            currentVertex = index3;
        }
        else
        {
            // Triangle is a cavity in the 2D shape.
            // The next triangle starts at the inside corner.
            currentVertex = index2;
        }
    }

    indices.shrink_to_fit();
    return indices;
}
