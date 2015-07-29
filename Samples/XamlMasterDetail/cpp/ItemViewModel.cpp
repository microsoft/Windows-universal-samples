#include "pch.h"
#include "ItemViewModel.h"

using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;
using namespace Windows::Globalization::DateTimeFormatting;

ItemViewModel^ ItemViewModel::FromItem(Item^ item)
{
    auto viewModel = ref new ItemViewModel();

    viewModel->m_itemId = item->Id;
    viewModel->m_dateCreated = item->DateCreated;
    viewModel->m_title = item->Title;
    viewModel->m_text = item->Text;

    return viewModel;
}

ItemViewModel::ItemViewModel()
{
}

Platform::String^ ItemViewModel::DateCreatedHourMinute::get()
{
    auto formatter = ref new DateTimeFormatter(L"hour minute");
    return formatter->Format(DateCreated);
}
