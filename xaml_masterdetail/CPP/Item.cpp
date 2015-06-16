#include "pch.h"
#include "Item.h"

using namespace MasterDetailApp::Data;

Item::Item()
{
}

Item::Item(int id, Windows::Foundation::DateTime dateCreated, Platform::String^ title, Platform::String^ text)
	: m_id(id), m_dateCreated(dateCreated), m_title(title), m_text(text)
{
}