﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "MainPage.xaml.h"

void ::App3::MainPage::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///MainPage.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::App3::MainPage::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
        case 1:
            {
                this->swapChainPanel = safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(__target);
            }
            break;
        case 2:
            {
                ::Windows::UI::Xaml::Controls::Button^ element2 = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(element2))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ItemPrevButton_Click);
            }
            break;
        case 3:
            {
                ::Windows::UI::Xaml::Controls::Button^ element3 = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(element3))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ItemNextButton_Click);
            }
            break;
        case 4:
            {
                this->cbShowWireframe = safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(this->cbShowWireframe))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::CheckBox_Click);
            }
            break;
        case 5:
            {
                this->cbShowLights = safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(this->cbShowLights))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::cbLights_Click);
            }
            break;
        case 6:
            {
                this->cbShowGrid = safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(this->cbShowGrid))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::cbGrid_Click);
            }
            break;
        case 7:
            {
                this->tbTitle = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
            }
            break;
        case 8:
            {
                this->tbFileSize = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
            }
            break;
        case 9:
            {
                this->tbVertex = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
            }
            break;
        case 10:
            {
                this->bottomAppBar = safe_cast<::Windows::UI::Xaml::Controls::AppBar^>(__target);
            }
            break;
        case 11:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element11 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element11))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::LoadFileButton_Click);
            }
            break;
        case 12:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element12 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element12))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::LoadFolderButton_Click);
            }
            break;
        case 13:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element13 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element13))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::AppBarButton_Click_1);
            }
            break;
        case 14:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element14 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element14))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::AppBarButton_Click_2);
            }
            break;
        case 15:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element15 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element15))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::AppBarButton_Click_3);
            }
            break;
        case 16:
            {
                ::Windows::UI::Xaml::Controls::AppBarButton^ element16 = safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::AppBarButton^>(element16))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::App3::MainPage::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::AppBarButton_Click_3);
            }
            break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::App3::MainPage::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}


