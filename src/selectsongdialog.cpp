// selectsongdialog.cpp

#include "selectsongdialog.h"
#include "./ui_selectsongdialog.h"

SelectSongDialog::SelectSongDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SelectSongDialog)
    , m_musicLibraryPath(FilePath::getProjectRootPath() + "/music_library/")
{
    ui->setupUi(this);
    ui->listWidget_SongsToSelect->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->listWidget_SongsToSelect->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->listWidget_SongsToSelect->setFocusPolicy(Qt::NoFocus);

    // CONNECT UI TO METHODS
    connect(ui->listWidget_SongsToSelect, &QListWidget::itemSelectionChanged, this, &SelectSongDialog::updateSelectedSongs);
    connect(ui->buttonBox_OkandCancel, &QDialogButtonBox::rejected, this, &SelectSongDialog::clearSelectedSongPaths);
}

SelectSongDialog::~SelectSongDialog()
{
    delete ui;
}

void SelectSongDialog::updateSelectedSongs()
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_SongsToSelect->selectedItems();
    m_selectedSongPaths.clear();
    for (QListWidgetItem* item : selectedItems) {
        m_selectedSongPaths.append(m_musicLibraryPath + item->text() + ".mp3");
    }
}

QStringList SelectSongDialog::getSelectedSongPaths()
{
    return m_selectedSongPaths;
}

void SelectSongDialog::displaySongs()
{
    ui->listWidget_SongsToSelect->clear();
    QStringList musicFilters;
    musicFilters << "*.mp3";

    QDir musicDir(m_musicLibraryPath);
    QFileInfoList musicFiles = musicDir.entryInfoList(musicFilters, QDir::Files);

    for (const QFileInfo& fileInfo : musicFiles) {
        QString songName = fileInfo.baseName();
        ui->listWidget_SongsToSelect->addItem(songName);
    }
}

void SelectSongDialog::clearSelectedSongPaths()
{
    m_selectedSongPaths.clear();
    close();
}