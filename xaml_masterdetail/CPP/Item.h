#pragma once

namespace MasterDetailApp { namespace Data {

	public ref class Item sealed
	{
	public:
		Item();
		Item(int id, Windows::Foundation::DateTime dateCreated, Platform::String^ title, Platform::String^ text);

		property int Id
		{
			int get() { return m_id; }
			void set(int value) { m_id = value; }
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

	internal:
		int m_id;
		Windows::Foundation::DateTime m_dateCreated;
		Platform::String^ m_title;
		Platform::String^ m_text;
	};

}}