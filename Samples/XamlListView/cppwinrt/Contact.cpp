//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "Contact.h"
#include "Contact.g.cpp"
#include <random>

using namespace winrt;
using namespace winrt::Windows::Foundation::Collections;
namespace
{
	auto& random_generator()
	{
		static std::mt19937 generator{ std::random_device{}() };
		return generator;
	}

	template<typename T>
	T random_in_range(T minValue, T maxValue)
	{
		return std::uniform_int_distribution<T>(minValue, maxValue)(random_generator());
	}

	template<typename T>
	T random_below(T limit)
	{
		return random_in_range(T{}, limit - 1);
	}

	template<typename T>
	T const& random_select(std::vector<T> const& source)
	{
		return source[random_below(source.size())];
	}
}

namespace winrt::SDKTemplate::implementation
{
	IObservableVector<SDKTemplate::Contact> Contact::GetContacts(int count)
	{
		std::vector<SDKTemplate::Contact> contacts;
		std::generate_n(std::back_inserter(contacts), count, &Contact::GetNewContact);
		return single_threaded_observable_vector(std::move(contacts));
	}

	IObservableVector<SDKTemplate::GroupInfoList> Contact::GetContactsGrouped(int count)
	{
		std::map<int, std::vector<SDKTemplate::Contact>> groupMap;

		// Generate the contacts and put each one in a group
		// based on the first letter of the last name.
		for (int i = 0; i < count; i++)
		{
			SDKTemplate::Contact contact = GetNewContact();
			auto upper = toupper(contact.LastName()[0]);
			groupMap[upper].push_back(contact);
		}

		// Generate the list of GroupInfoList object, each representing one group.
		std::vector<SDKTemplate::GroupInfoList> groups;
		for (auto&& [upper, values] : groupMap)
		{
			SDKTemplate::GroupInfoList group;
			group.Key(to_hstring(static_cast<char16_t>(upper)));
			group.ReplaceAll(std::move(values));
			groups.push_back(group);
		}
		return single_threaded_observable_vector(std::move(groups));
	}

	hstring Contact::GeneratePosition()
	{
		static std::vector<hstring> const positions({ L"Program Manager", L"Developer", L"Product Manager", L"Evangelist" });
		return random_select(positions);
	}

	hstring Contact::GenerateBiography()
	{
		static std::vector<hstring> const biographies({
			L"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer id facilisis lectus. Cras nec convallis ante, quis pulvinar tellus. Integer dictum accumsan pulvinar. Pellentesque eget enim sodales sapien vestibulum consequat.",
			L"Maecenas eu sapien ac urna aliquam dictum.",
			L"Nullam eget mattis metus. Donec pharetra, tellus in mattis tincidunt, magna ipsum gravida nibh, vitae lobortis ante odio vel quam.",
			L"Quisque accumsan pretium ligula in faucibus. Mauris sollicitudin augue vitae lorem cursus condimentum quis ac mauris. Pellentesque quis turpis non nunc pretium sagittis. Nulla facilisi. Maecenas eu lectus ante. Proin eleifend vel lectus non tincidunt. Fusce condimentum luctus nisi, in elementum ante tincidunt nec.",
			L"Aenean in nisl at elit venenatis blandit ut vitae lectus. Praesent in sollicitudin nunc. Pellentesque justo augue, pretium at sem lacinia, scelerisque semper erat. Ut cursus tortor at metus lacinia dapibus.",
			L"Ut consequat magna luctus justo egestas vehicula. Integer pharetra risus libero, et posuere justo mattis et.",
			L"Proin malesuada, libero vitae aliquam venenatis, diam est faucibus felis, vitae efficitur erat nunc non mauris. Suspendisse at sodales erat.",
			L"Aenean vulputate, turpis non tincidunt ornare, metus est sagittis erat, id lobortis orci odio eget quam. Suspendisse ex purus, lobortis quis suscipit a, volutpat vitae turpis.",
			L"Duis facilisis, quam ut laoreet commodo, elit ex aliquet massa, non varius tellus lectus et nunc. Donec vitae risus ut ante pretium semper. Phasellus consectetur volutpat orci, eu dapibus turpis. Fusce varius sapien eu mattis pharetra.",
			L"Nam vulputate eu erat ornare blandit. Proin eget lacinia erat. Praesent nisl lectus, pretium eget leo et, dapibus dapibus velit. Integer at bibendum mi, et fringilla sem."
			});
		return random_select(biographies);
	}

	hstring Contact::GeneratePhoneNumber()
	{
		return L"(" + to_hstring(random_in_range(100, 999)) + L") " + to_hstring(random_in_range(100, 999)) + L"-" + to_hstring(random_in_range(100, 9999));
	}

	hstring Contact::GenerateFirstName()
	{
		static std::vector<hstring> const names({
			L"Lilly", L"Mukhtar", L"Sophie", L"Femke", L"Abdul-Rafi'", L"Chirag-ud-D...", L"Mariana", L"Aarif", L"Sara",
			L"Ibadah", L"Fakhr", L"Ilene", L"Sardar", L"Hanna", L"Julie", L"Iain", L"Natalia", L"Henrik", L"Rasa", L"Quentin",
			L"Gadi", L"Pernille", L"Ishtar", L"Jimme", L"Justina", L"Lale", L"Elize", L"Rand", L"Roshanara", L"Rajab", L"Bijou",
			L"Marcus", L"Marcus", L"Alima", L"Francisco", L"Thaqib", L"Andreas", L"Mariana", L"Amalie", L"Rodney", L"Dena", L"Fadl",
			L"Ammar", L"Anna", L"Nasreen", L"Reem", L"Tomas", L"Filipa", L"Frank", L"Bari'ah", L"Parvaiz", L"Jibran", L"Tomas",
			L"Elli", L"Carlos", L"Diego", L"Henrik", L"Aruna", L"Vahid", L"Eliana", L"Roxane", L"Amanda", L"Ingrid", L"Wander",
			L"Malika", L"Basim", L"Eisa", L"Alina", L"Andreas", L"Deeba", L"Diya", L"Parveen", L"Bakr", L"Celine", L"Bakr",
			L"Marcus", L"Daniel", L"Mathea", L"Edmee", L"Hedda", L"Maria", L"Maja", L"Alhasan", L"Alina", L"Hedda", L"Victor",
			L"Aaftab", L"Guilherme", L"Maria", L"Kai", L"Sabien", L"Abdel", L"Fadl", L"Bahaar", L"Vasco", L"Jibran", L"Parsa",
			L"Catalina", L"Fouad", L"Colette"
			});
		return random_select(names);
	}

	hstring Contact::GenerateLastName()
	{
		static std::vector<hstring> const names({
			L"Carlson", L"Attia", L"Quint", L"Hollenberg", L"Khoury", L"Araujo", L"Hakimi", L"Seegers", L"Abadi", L"al",
			L"Krommenhoek", L"Siavashi", L"Kvistad", L"Sjo", L"Vanderslik", L"Fernandes", L"Dehli", L"Sheibani",
			L"Laamers", L"Batlouni", L"Lyngvær", L"Oveisi", L"Veenhuizen", L"Gardenier", L"Siavashi", L"Mutlu", L"Karzai",
			L"Mousavi", L"Natsheh", L"Seegers", L"Nevland", L"Lægreid", L"Bishara", L"Cunha", L"Hotaki", L"Kyvik",
			L"Cardoso", L"Pilskog", L"Pennekamp", L"Nuijten", L"Bettar", L"Borsboom", L"Skistad", L"Asef", L"Sayegh",
			L"Sousa", L"Medeiros", L"Kregel", L"Shamoun", L"Behzadi", L"Kuzbari", L"Ferreira", L"Van", L"Barros",
			L"Fernandes", L"Formo", L"Nolet", L"Shahrestaani", L"Correla", L"Amiri", L"Sousa", L"Fretheim", L"Van",
			L"Hamade", L"Baba", L"Mustafa", L"Bishara", L"Formo", L"Hemmati", L"Nader", L"Hatami", L"Natsheh", L"Langen",
			L"Maloof", L"Berger", L"Ostrem", L"Bardsen", L"Kramer", L"Bekken", L"Salcedo", L"Holter", L"Nader", L"Bettar",
			L"Georgsen", L"Cunha", L"Zardooz", L"Araujo", L"Batalha", L"Antunes", L"Vanderhoorn", L"Nader", L"Abadi",
			L"Siavashi", L"Montes", L"Sherzai", L"Vanderschans", L"Neves", L"Sarraf", L"Kuiters"
			});
		return random_select(names);
	}

}