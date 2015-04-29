#include "pch.h"
#include "ItemsDataSource.h"

using namespace MasterDetailApp::Data;
using namespace Platform::Collections;
using namespace Windows::Foundation;

ItemsDataSource::ItemsDataSource()
{
}

Vector<Item^>^ ItemsDataSource::s_items = ref new Vector<Item^>{
	ref new Item(
		0,
		DateTime(),
		L"Item 1",
		R"("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer id facilisis lectus. Cras nec convallis ante, quis pulvinar tellus. Integer dictum accumsan pulvinar. Pellentesque eget enim sodales sapien vestibulum consequat.

Maecenas eu sapien ac urna aliquam dictum.

Nullam eget mattis metus. Donec pharetra, tellus in mattis tincidunt, magna ipsum gravida nibh, vitae lobortis ante odio vel quam."
		)"
		),
	ref new Item(
		1,
		DateTime(),
		L"Item 2",
		R"(Quisque accumsan pretium ligula in faucibus. Mauris sollicitudin augue vitae lorem cursus condimentum quis ac mauris. Pellentesque quis turpis non nunc pretium sagittis. Nulla facilisi. Maecenas eu lectus ante. Proin eleifend vel lectus non tincidunt. Fusce condimentum luctus nisi, in elementum ante tincidunt nec.

Aenean in nisl at elit venenatis blandit ut vitae lectus. Praesent in sollicitudin nunc. Pellentesque justo augue, pretium at sem lacinia, scelerisque semper erat. Ut cursus tortor at metus lacinia dapibus."
		)"
		),
	ref new Item(
		2,
		DateTime(),
		L"Item 3",
		R"(Ut consequat magna luctus justo egestas vehicula. Integer pharetra risus libero, et posuere justo mattis et.

Proin malesuada, libero vitae aliquam venenatis, diam est faucibus felis, vitae efficitur erat nunc non mauris. Suspendisse at sodales erat.
Aenean vulputate, turpis non tincidunt ornare, metus est sagittis erat, id lobortis orci odio eget quam. Suspendisse ex purus, lobortis quis suscipit a, volutpat vitae turpis.
		)"
		),
	ref new Item(
		3,
		DateTime(),
		L"Item 4",
		R"(Duis facilisis, quam ut laoreet commodo, elit ex aliquet massa, non varius tellus lectus et nunc. Donec vitae risus ut ante pretium semper. Phasellus consectetur volutpat orci, eu dapibus turpis. Fusce varius sapien eu mattis pharetra.

Nam vulputate eu erat ornare blandit. Proin eget lacinia erat. Praesent nisl lectus, pretium eget leo et, dapibus dapibus velit. Integer at bibendum mi, et fringilla sem.
		)"
		)
};