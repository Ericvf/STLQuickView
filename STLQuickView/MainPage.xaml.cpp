//
// DirectXPage.xaml.cpp
// Implementation of the DirectXPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace App3;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;

MainPage::MainPage() :
	m_windowVisible(true),
	m_coreInput(nullptr)
{
	InitializeComponent();

	OutputDebugString(L"MainPage\n");

	// Register event handlers for page lifecycle.
	CoreWindow^ window = Window::Current->CoreWindow;

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &MainPage::OnVisibilityChanged);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnDisplayContentsInvalidated);

	swapChainPanel->CompositionScaleChanged +=
		ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &MainPage::OnCompositionScaleChanged);

	swapChainPanel->SizeChanged +=
		ref new SizeChangedEventHandler(this, &MainPage::OnSwapChainPanelSizeChanged);

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
	m_deviceResources->SetSwapChainPanel(swapChainPanel);

	// Register our SwapChainPanel to get independent input pointer events
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^)
	{
		// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
		m_coreInput = swapChainPanel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen
		);

		// Register for pointer events, which will be raised on the background thread.
		m_coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerPressed);
		m_coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerMoved);
		m_coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerReleased);
		m_coreInput->PointerWheelChanged += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerWheelChanged);

		// Begin processing input messages as they're delivered.
		m_coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	});

	// Run task on a dedicated high priority background thread.
	m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

	m_main = std::shared_ptr<MainGame>(new MainGame(m_deviceResources));
	m_main->StartRenderLoop();

	StartTimerAndRegisterHandler();
}

MainPage::~MainPage()
{
	// Stop rendering and processing events on destruction.
	m_main->StopRenderLoop();
	m_coreInput->Dispatcher->StopProcessEvents();
}

void MainPage::SaveInternalState(IPropertySet^ state)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->Trim();

	// Stop rendering when the app is suspended.
	m_main->StopRenderLoop();

	// Put code to save app state here.
}

void MainPage::LoadInternalState(IPropertySet^ state)
{
	// Put code to load app state here.

	// Start rendering when the app is resumed.
	m_main->StartRenderLoop();
}

void MainPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
	if (m_windowVisible)
	{
		m_main->StartRenderLoop();
	}
	else
	{
		m_main->StopRenderLoop();
	}
}

void MainPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	// Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
	// if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
	// you should always retrieve it using the GetDpi method.
	// See DeviceResources.cpp for more details.
	m_deviceResources->SetDpi(sender->LogicalDpi);
	m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->ValidateDevice();
}

void MainPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
	auto properties = e->CurrentPoint->Properties;

	m_main->StartTracking(
		e->CurrentPoint->Position.X,
		e->CurrentPoint->Position.Y,
		properties->IsRightButtonPressed);
}

void MainPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
	m_main->TrackingUpdate(e->CurrentPoint->Position.X, e->CurrentPoint->Position.Y);
}

void MainPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
	m_main->StopTracking();
}

void MainPage::OnPointerWheelChanged(Object^ sender, PointerEventArgs^ e)
{
	m_main->ZoomUpdate(e->CurrentPoint->Properties->MouseWheelDelta);
}

void MainPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
	m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetLogicalSize(e->NewSize);
	m_main->CreateWindowSizeDependentResources();
}

void MainPage::StartTimerAndRegisterHandler() {
	auto timer = ref new Windows::UI::Xaml::DispatcherTimer();
	TimeSpan ts;
	ts.Duration = 100;
	timer->Interval = ts;
	timer->Start();
	auto registrationtoken = timer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnRendering);
}

void MainPage::OnRendering(Platform::Object^ sender, Platform::Object^ args) {
	auto fps = m_main->GetFps();
	//this->tbTitle->Text = fps.ToString();
}

void MainPage::CheckBox_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto isEnabled = this->cbShowWireframe->IsChecked->Value;
	m_main->SetWireframe(isEnabled);
}

void MainPage::cbLights_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto isEnabled = this->cbShowLights->IsChecked->Value;
	m_main->SetShowLights(isEnabled);
}

void MainPage::cbGrid_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto isEnabled = this->cbShowGrid->IsChecked->Value;
	m_main->SetShowGrid(isEnabled);
}

void MainPage::AppBarButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->Reset(400, 35);
}

void MainPage::AppBarButton_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->Reset(125, 75);
}

void MainPage::AppBarButton_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->Reset(-400, 35);
}

void MainPage::LoadMostRecentFileOrDirectory() {
	auto entries = StorageApplicationPermissions::MostRecentlyUsedList->Entries;
	if (entries->Size > 0)
	{
		auto entry = (AccessListEntry)entries->GetAt(0);

		if (StorageApplicationPermissions::MostRecentlyUsedList->ContainsItem(entry.Token))
		{
			create_task(StorageApplicationPermissions::MostRecentlyUsedList->GetItemAsync(entry.Token))
				.then([&](IStorageItem^ item)
			{
				auto storageFile = dynamic_cast<StorageFile^>(item);
				if (storageFile != nullptr)
					LoadFile(storageFile);
				
				auto storageFolder = dynamic_cast<StorageFolder^>(item);
				if (storageFolder != nullptr)
					LoadFolder(storageFolder);
			});
		}
	}
}

void MainPage::LoadFromFileActivation(StorageFile^ file) {
	hasFolderLoaded = false;
	LoadFile(file);
}

void MainPage::SetFolderMode(bool isFolderMode)
{
	hasFolderLoaded = isFolderMode;
}

void MainPage::LoadFile(StorageFile^ file)
{
	if (!hasFolderLoaded) {
		StorageApplicationPermissions::MostRecentlyUsedList->Clear();
		StorageApplicationPermissions::MostRecentlyUsedList->Add(file, file->Name, RecentStorageItemVisibility::AppOnly);
		OutputDebugString(L"StorageApplicationPermissions\n");
	}

	this->tbTitle->Text = file->Name;
	m_main->LoadFileActive(file);
}

void MainPage::LoadFolder(StorageFolder^ folder)
{
	using namespace Windows::Storage;
	using namespace Windows::Storage::Search;
	using namespace Platform;
	using namespace Platform::Collections;

	StorageApplicationPermissions::MostRecentlyUsedList->Clear();
	StorageApplicationPermissions::MostRecentlyUsedList->Add(folder, folder->Name, RecentStorageItemVisibility::AppOnly);
	OutputDebugString(L"LoadFolder StorageApplicationPermissions\n");

	hasFolderLoaded = true;

	// Set up file type filter.
	auto fileTypeFilter = ref new Vector<String^>();
	fileTypeFilter->Append(".stl");

	// Create query options.
	auto queryOptions = ref new QueryOptions(CommonFileQuery::OrderByName, fileTypeFilter);
	auto query = folder->CreateFileQueryWithOptions(queryOptions);

	create_task(query->GetFilesAsync())
		.then([this](Windows::Foundation::Collections::IVectorView<StorageFile^>^ files)
	{
		if (files->Size > 0) {
			folderItems = files;
			currentFileIndex = 0;
			LoadFile(folderItems->GetAt(currentFileIndex));
		}
	});

	this->tbTitle->Text = folder->Name;
}

void MainPage::LoadFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	FileOpenPicker^ openPicker = ref new FileOpenPicker();
	openPicker->ViewMode = PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".stl");

	create_task(openPicker->PickSingleFileAsync())
		.then([this](StorageFile^ file)
	{
		hasFolderLoaded = false;
		LoadFile(file);
	});
}

void MainPage::LoadFolderButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	auto openPicker = ref new FolderPicker();
	openPicker->ViewMode = PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".stl");

	create_task(openPicker->PickSingleFolderAsync())
		.then([this](StorageFolder^ folder)
	{
		LoadFolder(folder);
	});
}

void MainPage::ItemPrevButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (currentFileIndex > 0)
	{
		currentFileIndex--;
		LoadFile(folderItems->GetAt(currentFileIndex));
	}
}

void MainPage::ItemNextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (currentFileIndex < folderItems->Size - 1)
	{
		currentFileIndex++;
		LoadFile(folderItems->GetAt(currentFileIndex));
	}
}
