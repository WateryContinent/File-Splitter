#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <fstream>
#include <sstream>

// Declare MyFrame class for the main application window
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

    void SplitFile(const wxString& filePath, const wxString& outputDir, long chunkSize);
    void AssembleFiles(const wxString& inputDir);

private:
    void OnButtonSplitClick(wxCommandEvent& event);
    void OnButtonAssembleClick(wxCommandEvent& event);
    void OnButtonAdjustSizeClick(wxCommandEvent& event);

    long chunkSize; // Store the split size in MB
};

// A simple dialog for adjusting the split size
class SplitSizeDialog : public wxDialog {
public:
    SplitSizeDialog(wxWindow* parent, long* chunkSize);

private:
    void OnOK(wxCommandEvent& event);

    long* chunkSize; // Pointer to the chunk size value in the main frame
    wxTextCtrl* sizeInput;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
    
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame("File Splitter / Assembler");
    wxMenuBar* menubar = new wxMenuBar();
    wxMenu* testmenu = new  wxMenu();
    
    testmenu->Append(wxID_ABOUT, _("About"));
    testmenu->Append(wxID_HELP, _("Help"));
    testmenu->Append(wxID_PREFERENCES, _("Preferences"));
    testmenu->Append(wxID_EXIT, _("Exit"));
    
    frame->SetMenuBar(menubar);
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)), chunkSize(10) { // Default split size: 10 MB
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Set a title to the main window
    SetTitle("File Splitter and Assembler");

    // Create a vertical box sizer to center elements
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    
    // Add a title text above the buttons
    wxStaticText* titleText = new wxStaticText(panel, wxID_ANY, "File Splitter/Assembler");
    titleText->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)); // Bold and larger font
    vbox->Add(titleText, 0, wxALL | wxALIGN_CENTER, 10);

    wxButton* buttonSplit = new wxButton(panel, wxID_ANY, "Split File");
    wxButton* buttonAssemble = new wxButton(panel, wxID_ANY, "Assemble File");
    wxButton* buttonAdjustSize = new wxButton(panel, wxID_ANY, "Adjust Split Size");

    // Bind events to their handlers
    Bind(wxEVT_BUTTON, &MyFrame::OnButtonSplitClick, this, buttonSplit->GetId());
    Bind(wxEVT_BUTTON, &MyFrame::OnButtonAssembleClick, this, buttonAssemble->GetId());
    Bind(wxEVT_BUTTON, &MyFrame::OnButtonAdjustSizeClick, this, buttonAdjustSize->GetId());

    // Add buttons to sizer and center them
    vbox->Add(buttonSplit, 0, wxALL | wxALIGN_CENTER, 15);
    vbox->Add(buttonAssemble, 0, wxALL | wxALIGN_CENTER, 15);
    vbox->Add(buttonAdjustSize, 0, wxALL | wxALIGN_CENTER, 15);

    // Center the entire vertical box sizer within the panel
    vbox->AddStretchSpacer(1); // Add a spacer to push buttons to the center vertically
    panel->SetSizer(vbox);
}

void MyFrame::OnButtonSplitClick(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Select a file to split", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString selectedFile = openFileDialog.GetPath();
        wxDirDialog dirDialog(this, "Select output directory", "", wxDD_DEFAULT_STYLE);
        if (dirDialog.ShowModal() == wxID_OK) {
            wxString outputDir = dirDialog.GetPath();
            SplitFile(selectedFile, outputDir, chunkSize * 1024 * 1024); // Convert MB to bytes
        }
    }
}

void MyFrame::OnButtonAssembleClick(wxCommandEvent& event) {
    wxDirDialog dirDialog(this, "Select the directory with split files", "", wxDD_DEFAULT_STYLE);
    if (dirDialog.ShowModal() == wxID_OK) {
        wxString inputDir = dirDialog.GetPath();
        AssembleFiles(inputDir);
    }
}

void MyFrame::OnButtonAdjustSizeClick(wxCommandEvent& event) {
    // Open the split size adjustment dialog
    SplitSizeDialog sizeDialog(this, &chunkSize);
    sizeDialog.ShowModal();
}

void MyFrame::SplitFile(const wxString& filePath, const wxString& outputDir, long chunkSize) {
    std::ifstream inputFile(filePath.ToStdString(), std::ios::binary);
    if (!inputFile) {
        wxLogError("Could not open file for splitting.");
        return;
    }

    wxFileName fileName(filePath);
    wxString baseName = fileName.GetName(); // Original file name without extension
    wxString extension = fileName.GetExt(); // Get file extension

    std::ofstream logFile(wxString::Format("%s/split_log.txt", outputDir).ToStdString());
    if (!logFile) {
        wxLogError("Could not create log file.");
        return;
    }
    logFile << baseName << "." << extension << std::endl;

    char* buffer = new char[chunkSize];
    long partNum = 0;

    while (inputFile.read(buffer, chunkSize) || inputFile.gcount() > 0) {
        wxString partFileName = wxString::Format("%s/%s_part%03ld.dat", outputDir, baseName, partNum++);
        std::ofstream outputFile(partFileName.ToStdString(), std::ios::binary);
        if (!outputFile) {
            wxLogError("Could not create output file.");
            break;
        }
        outputFile.write(buffer, inputFile.gcount());
        outputFile.close();
    }

    delete[] buffer;
    inputFile.close();
    logFile.close();

    wxMessageBox("File splitting completed successfully!", "Success", wxOK | wxICON_INFORMATION);
}

void MyFrame::AssembleFiles(const wxString& inputDir) {
    std::ifstream logFile(wxString::Format("%s/split_log.txt", inputDir).ToStdString());
    if (!logFile) {
        wxLogError("Could not open log file.");
        return;
    }

    std::string originalFileNameStd;
    std::getline(logFile, originalFileNameStd);
    wxString originalFileName(originalFileNameStd);
    logFile.close();

    std::ofstream outputFile(wxString::Format("%s/%s", inputDir, originalFileName).ToStdString(), std::ios::binary);
    if (!outputFile) {
        wxLogError("Could not create output file.");
        return;
    }

    long partNum = 0;
    while (true) {
        wxString partFileName = wxString::Format("%s/%s_part%03ld.dat", inputDir, originalFileName.BeforeLast('.'), partNum++);
        std::ifstream inputFile(partFileName.ToStdString(), std::ios::binary);

        if (!inputFile) {
            break;
        }

        outputFile << inputFile.rdbuf();
        inputFile.close();
    }

    outputFile.close();

    wxMessageBox("File assembling completed successfully!", "Success", wxOK | wxICON_INFORMATION);
}

// Split size adjustment dialog implementation
SplitSizeDialog::SplitSizeDialog(wxWindow* parent, long* chunkSize)
    : wxDialog(parent, wxID_ANY, "Adjust Split Size", wxDefaultPosition, wxSize(300, 150)), chunkSize(chunkSize) {

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(this, wxID_ANY, "Enter split size in MB:");
    vbox->Add(label, 0, wxALL | wxALIGN_CENTER, 10);

    // Make the text box smaller
    sizeInput = new wxTextCtrl(this, wxID_ANY, wxString::Format("%ld", *chunkSize), wxDefaultPosition, wxSize(100, -1));
    vbox->Add(sizeInput, 0, wxALL | wxALIGN_CENTER, 10);

    wxButton* okButton = new wxButton(this, wxID_OK, "OK");
    vbox->Add(okButton, 0, wxALL | wxALIGN_CENTER, 10);

    SetSizer(vbox);

    Bind(wxEVT_BUTTON, &SplitSizeDialog::OnOK, this, wxID_OK);
}

void SplitSizeDialog::OnOK(wxCommandEvent& event) {
    long newSize = 0;
    if (sizeInput->GetValue().ToLong(&newSize) && newSize > 0) {
        *chunkSize = newSize; // Update the split size
    }
    EndModal(wxID_OK); // Close the dialog
}
