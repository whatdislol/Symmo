#ifndef SELECTSONGDIALOG_H
#define SELECTSONGDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QListWidget>
#include <QDir>
#include <QFileInfo>

namespace Ui {
    class SelectSongDialog;
}

class SelectSongDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectSongDialog(QString musicLibraryPath, QWidget* parent = nullptr);
    ~SelectSongDialog();

    void updateSelectedSongs();
    QStringList getSelectedSongPaths();
    void displaySongs();
    void clearSelectedSongPaths();

private:
    Ui::SelectSongDialog* ui;
    QStringList m_selectedSongPaths;
    QString m_musicLibraryPath;
};

#endif // SELECTSONGDIALOG_H