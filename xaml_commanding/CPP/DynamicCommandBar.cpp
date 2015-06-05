//
// DynamicCommandBar.cpp
// Implementation of the DynamicCommandBar class.
//

#include "pch.h"
#include "DynamicCommandBar.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

namespace Commanding
{
	DependencyProperty^ DynamicCommandBar::_contentMinWidthProperty =
		DependencyProperty::Register(
			"ContentMinWidth",
			TypeName(double::typeid),
			TypeName(DynamicCommandBar::typeid),
			ref new PropertyMetadata(0.0));

	void DynamicCommandBar::OnApplyTemplate()
	{
		CommandBar::OnApplyTemplate();
		_moreButton = dynamic_cast<Button^>(GetTemplateChild("ExpandButton"));
	}

	Size DynamicCommandBar::MeasureOverride(Size availableSize)
	{
		static const Size infiniteBounds = { std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() };
		auto sizeToReport = CommandBar::MeasureOverride(availableSize);

		// Account for the size of the Content area
		auto contentWidth = this->ContentMinWidth;

		// Account for the size of the More button 
		auto expandButtonWidth = 0.0;
		if (_moreButton != nullptr && _moreButton->Visibility == Windows::UI::Xaml::Visibility::Visible)
		{
			expandButtonWidth = _moreButton->DesiredSize.Width;
		}

		double requestedWidth = expandButtonWidth + contentWidth;

		// Include the size of all the PrimaryCommands
		for (ICommandBarElement^ cmd : PrimaryCommands)
		{
			auto uie = dynamic_cast<UIElement^>(cmd);
			requestedWidth += uie->DesiredSize.Width;
		}

		// First, move items to the overflow until the remaining PrimaryCommands fit
		for (auto i = PrimaryCommands->Size - 1; i >= 0 && requestedWidth > availableSize.Width; i--)
		{
			auto item = PrimaryCommands->GetAt(i);
			auto element = dynamic_cast<UIElement^>(item);

			if (element == nullptr)
			{
				continue;
			}

			requestedWidth -= element->DesiredSize.Width;

			if (overflow.size() == 0 && SecondaryCommands->Size > 0 && dynamic_cast<AppBarSeparator^>(element) == nullptr)
			{
				// Insert a separator to differentiate between the items that were already in the overflow versus
				// those we moved
				auto abs = ref new AppBarSeparator();
				abs->Measure(infiniteBounds);
				separatorQueue.push(std::tuple<ICommandBarElement^, double>(abs, abs->DesiredSize.Width));
				separatorQueueWidth += abs->DesiredSize.Width;
			}

			PrimaryCommands->RemoveAt(i);

			// move separators we've queued up 
			while (!separatorQueue.empty())
			{
				auto next = separatorQueue.front();
				separatorQueue.pop();

				separatorQueueWidth -= std::get<1>(next);
				overflow.push(next);
				SecondaryCommands->InsertAt(0, std::get<0>(next));
			}

			// We store the measured size before it moves to overflow and will rely on that value
			// when determining how many items to move back out the overflow.
			overflow.push(std::tuple<ICommandBarElement^, double>(item, element->DesiredSize.Width));

			SecondaryCommands->InsertAt(0, reinterpret_cast<ICommandBarElement^>(element));

			// if a separator was adjacent to the one we removed then move the separator to our holding queue so 
			// that it doesn't appear without actually separating the content
			AppBarSeparator^ last = nullptr;
			if (PrimaryCommands->Size > 0)
			{
				auto item = PrimaryCommands->GetAt(PrimaryCommands->Size - 1);
				last = dynamic_cast<AppBarSeparator^>(item);
			}

			if (last != nullptr)
			{
				PrimaryCommands->RemoveAtEnd();
				separatorQueue.push(std::tuple<ICommandBarElement^, double>(last, last->DesiredSize.Width));
				separatorQueueWidth += last->DesiredSize.Width;
				requestedWidth -= last->DesiredSize.Width;
			}
		}

		// Next move items out of the overflow if room is available
		while (overflow.size() > 0 && requestedWidth + separatorQueueWidth + std::get<1>(overflow.top()) <= availableSize.Width)
		{
			auto top = overflow.top();
			auto topItem = std::get<0>(top);
			auto topWidth = std::get<1>(top);
			overflow.pop();

			SecondaryCommands->RemoveAt(0);

			if (overflow.size() == 1 && SecondaryCommands->Size > 1)
			{
				// must be the separator injected earlier	
				overflow.pop();
				SecondaryCommands->RemoveAt(0);
			}

			if (dynamic_cast<AppBarSeparator^>(topItem) != nullptr)
			{
				separatorQueue.push(top);
				separatorQueueWidth += topWidth;
				continue;
			}
			else
			{
				while (separatorQueue.size() > 0)
				{
					auto next = separatorQueue.front();
					auto nextItem = std::get<0>(next);
					auto nextWidth = std::get<1>(next);
					separatorQueue.pop();

					PrimaryCommands->Append(nextItem);
					separatorQueueWidth -= nextWidth;
					requestedWidth += nextWidth;
				}
			}

			// Sometimes this property is being set to disabled
			auto ctrl = reinterpret_cast<Control^>(topItem);
			ctrl->IsEnabled = true;

			PrimaryCommands->Append(topItem);
			requestedWidth += topWidth;

			// check to see if after moving this item we leave a separator at the top of the overflow
			if (overflow.size() > 0)
			{
				if (dynamic_cast<AppBarSeparator^>(std::get<0>(overflow.top())) != nullptr)
				{
					// we won't leave an orphaned separator
					SecondaryCommands->RemoveAt(0);
					auto top = overflow.top();
					overflow.pop();

					auto element = dynamic_cast<UIElement^>(topItem);
					element->Measure(infiniteBounds);
					separatorQueue.push(std::tuple<ICommandBarElement^, double>(std::get<0>(top), element->DesiredSize.Width));
					separatorQueueWidth += element->DesiredSize.Width;
				}
			}
		}

		return sizeToReport;
	}
}
