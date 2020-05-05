using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Storage;
using Windows.Storage.Pickers;

namespace FileManager
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        /// <summary>
        /// Collection for binding to file manager listview (private field)
        /// </summary>
        private ObservableCollection<FileManagerItem> fileManagerItemsCollection;

        /// <summary>
        /// Current folder in file manager (private field)
        /// </summary>
        private StorageFolder currentFolder;

        /// <summary>
        /// Collection for binding to file manager listview (public property)
        /// </summary>
        public ObservableCollection<FileManagerItem> FileManagerItems
        {
            get
            {
                return fileManagerItemsCollection;
            }
            private set
            {
                fileManagerItemsCollection = value;
            }
        }

        /// <summary>
        /// Current folder in file manager (public property)
        /// </summary>
        public StorageFolder CurrentFolder
        {
            get
            {
                return currentFolder;
            }
            private set
            {
                currentFolder = value;   
            }
        }

        public MainPage()
        {
            this.InitializeComponent();
            // set default current folder
            CurrentFolder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            FileManagerItems = new ObservableCollection<FileManagerItem>(); // create collection to binding
            foldersList.ItemsSource = FileManagerItems; // bind collection to items source
            FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
            DisableFileManageBtns(); // disable all buttons for files manage (copy, delete)
        }

        /// <summary>
        /// Click to item in listview handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void FoldersList_ItemClick(object sender, ItemClickEventArgs e)
        {
            FileManagerItem item = e.ClickedItem as FileManagerItem; // get and unbox clicked item
            if(item == null)
            {
                ShowMsgBox("Error!", "Choose element");
                return;
            }
            if(item.Size == "") // check if this is a folder. it works cause folders dont have size
            {                   // if it is folder, than open it
                DisableFileManageBtns(); // disable all buttons for files manage (copy, delete)
                CurrentFolder = await CurrentFolder.GetFolderAsync(item.Name); // get subfolder
                FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
            }
            else // then selected item is file, than enable btns to files manage 
            {
                EnableFileManageBtns();
            }
        }

        /// <summary>
        /// disable all buttons for files manage (copy, delete)
        /// </summary>
        private void DisableFileManageBtns()
        {
            copyFile.IsEnabled = false;
            deleteFile.IsEnabled = false;
        }

        /// <summary>
        /// enable all buttons for files manage (copy, delete)
        /// </summary>
        private void EnableFileManageBtns()
        {
            copyFile.IsEnabled = true;
            deleteFile.IsEnabled = true;
        }

        /// <summary>
        /// Go parrent folder handler 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void goParentFolder_btn_click(object sender, RoutedEventArgs e)
        {
            StorageFolder childFolder = CurrentFolder; // save current folder object
            CurrentFolder = await CurrentFolder.GetParentAsync(); // try get parent folder
            if(CurrentFolder == null) // check for null
            { // if it is null, then restore prev current folder, and show msg about error
                currentFolder = childFolder;
                ShowMsgBox("Error!", "You dont have access to parrent folder\n");
                return;
            }
            else
                FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
        }

        /// <summary>
        /// Show message box 
        /// </summary>
        /// <param name="title"></param>
        /// <param name="msg"></param>
        private async void ShowMsgBox(string title, string msg)
        {
            ContentDialog contentDialog = new ContentDialog()
            {
                Title = title,
                Content = msg,
                CloseButtonText = "OK"
            };
            await contentDialog.ShowAsync();
        }

        /// <summary>
        /// Exit from application handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exit_btn_click(object sender, RoutedEventArgs e)
        {
            App.Current.Exit();
        }

        /// <summary>
        /// Method for updating file manager items collection
        /// </summary>
        /// <param name="folder"></param>
        private async void FillOutFileManagerItemsCollection(StorageFolder folder)
        {
            FileManagerItems.Clear();

            // get all subfolders
            IReadOnlyList<StorageFolder> allSubfolders = await folder.GetFoldersAsync();

            foreach(var subFolder in allSubfolders)
            {
                FileManagerItems.Add(new FileManagerItem()
                {
                    Name = subFolder.Name,
                    Size = "",
                    CreationDate = subFolder.DateCreated.ToString("yyyy:mm:dd hh:mm")
                });
            }

            // get all files
            IReadOnlyList<StorageFile> allFiles = await folder.GetFilesAsync();

            foreach (var file in allFiles)
            {
                var basicProp = await file.GetBasicPropertiesAsync();

                FileManagerItems.Add(new FileManagerItem()
                {
                    Name = file.Name,
                    Size = SizeInBytesToOptimalUnits(basicProp.Size),
                    CreationDate = file.DateCreated.ToString("yyyy:mm:dd hh:mm")
                });
            }

            currentFolderPath_tblock.Text = CurrentFolder.Path;
        }

        /// <summary>
        /// Get size in optimal units
        /// </summary>
        /// <param name="sizeInBytes"></param>
        /// <returns></returns>
        private string SizeInBytesToOptimalUnits(ulong sizeInBytes)
        {
            string[] unitsNames = { "B", "KB", "MB", "GB", "TB"};
            int unitIndex = 0;
            
            while(sizeInBytes >= 1024 && unitIndex < unitsNames.Length)
            {
                unitIndex++;
                sizeInBytes /= 1024;
            }

            return sizeInBytes.ToString() + " " + unitsNames[unitIndex];
        }

        /// <summary>
        /// Open new folder handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void OpenFolderDialog_btn_Click(object sender, RoutedEventArgs e)
        {
            FolderPicker folderPicker = new FolderPicker();
            folderPicker.ViewMode = PickerViewMode.List;
            folderPicker.SuggestedStartLocation = PickerLocationId.ComputerFolder;
            folderPicker.FileTypeFilter.Add("*");
            StorageFolder newFolder = await folderPicker.PickSingleFolderAsync();
            if(newFolder != null)
            {
                CurrentFolder = newFolder;
                FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
            }
        }

        /// <summary>
        /// delete file handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void DeleteFile_Click(object sender, RoutedEventArgs e)
        {
            FileManagerItem item = foldersList.SelectedItem as FileManagerItem;
            if(item != null)
            {
                // get file
                StorageFile file = await CurrentFolder.GetFileAsync(item.Name);
                if(file != null)
                {
                    // delete file 
                    await file.DeleteAsync();
                    FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
                }
            }
        }

        /// <summary>
        /// Copy file click handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CopyFile_Click(object sender, RoutedEventArgs e)
        {
            FileManagerItem item = foldersList.SelectedItem as FileManagerItem; // update file manager items collection
            if (item != null) 
            {
                // get file
                StorageFile file = await CurrentFolder.GetFileAsync(item.Name);
                if (file != null)
                {
                    // copy to the current folder file
                    await file.CopyAsync(CurrentFolder, file.DisplayName+"_copy"+file.FileType);
                    FillOutFileManagerItemsCollection(CurrentFolder); // update file manager items collection
                }
            }
        }
    }

    /// <summary>
    /// Class for binding to listview 
    /// </summary>
    public class FileManagerItem
    {
        public string Name { get; set; }
        public string Size { get; set; }
        public string CreationDate { get; set; }
    }
}
