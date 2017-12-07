#pragma once
namespace SDKTemplate
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class ItemDetails sealed
    {
    public:
        ItemDetails(Windows::Services::Store::StoreProduct^ product)
        {
            title = product->Title;
            price = product->Price->FormattedPrice;
            inCollection = product->IsInUserCollection;
            productKind = product->ProductKind;
            storeId = product->StoreId;
        }

        property Platform::String^ Title { Platform::String^ get() { return title; }}
        property Platform::String^ Price { Platform::String^ get() { return price; }}
        property boolean InCollection { boolean get() { return inCollection; }}
        property Platform::String^ ProductKind { Platform::String^ get() { return productKind; }}
        property Platform::String^ StoreId { Platform::String^ get() { return storeId; }}

        property Platform::String^ FormattedTitle
        {
            Platform::String^ get()
            {
                return Title + " (" + ProductKind + ") " + Price + ", InUserCollection:" + (InCollection ? "True" : "False");
            }
        }

    private:
        Platform::String^ title;
        Platform::String^ price;
        boolean inCollection;
        Platform::String^ productKind;
        Platform::String^ storeId;
    };
}
