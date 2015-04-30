#pragma once

namespace MasterDetailApp { namespace Data {


	public ref class ItemsDataSource sealed
	{
	public:
		static Windows::Foundation::Collections::IVector<Item^>^ GetAllItems()
		{
			return s_items;
		}

		static Item^ GetItemById(int id)
		{
			return s_items->GetAt(id);
		}

	private:
		ItemsDataSource();

		static Platform::Collections::Vector<Item^>^ s_items;
	};

}}