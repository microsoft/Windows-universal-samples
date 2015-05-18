#pragma once


namespace MasterDetailApp { namespace ViewModels {

	public ref class ItemViewModel sealed
	{
	public:
		static ItemViewModel^ FromItem(::MasterDetailApp::Data::Item^ item);

		property int ItemId
		{
			int get() { return m_itemId; }
		}

		property Windows::Foundation::DateTime DateCreated
		{
			Windows::Foundation::DateTime get() { return m_dateCreated; }
			void set(Windows::Foundation::DateTime value) { m_dateCreated = value; }
		}

		property Platform::String^ Title
		{
			Platform::String^ get() { return m_title; }
			void set(Platform::String^ value) { m_title = value; }
		}

		property Platform::String^ Text
		{
			Platform::String^ get() { return m_text; }
			void set(Platform::String^ value) { m_text = value; }
		}

		property Platform::String^ DateCreatedHourMinute
		{
			Platform::String^ get();
		}

	private:
		ItemViewModel();

		int m_itemId;
		Platform::String^ m_title;
		Platform::String^ m_text;
		Windows::Foundation::DateTime m_dateCreated;
	};

}}