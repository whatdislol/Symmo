// selectsongdialog.cpp

#include "selectsongdialog.h"
#include "./ui_selectsongdialog.h"

SelectSongDialog::SelectSongDialog(const QStringList& availableSongs, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectSongDialog)
{
    ui->setupUi(this);
}

SelectSongDialog::~SelectSongDialog()
{
    delete ui;
}

QStringList SelectSongDialog::getSelectedSongs() const
{
    QStringList selectedSongs;
    // Get the selected items from the list widget and add them to the list
    //QList<QListWidgetItem*> selectedItems = ui->listWidget->selectedItems();
    //for (QListWidgetItem* item : selectedItems) {
    //    selectedSongs.append(item->text());
    // }
    //return selectedSongs;
    return QStringList();
}