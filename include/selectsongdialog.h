// selectsongdialog.h

#ifndef SELECTSONGDIALOG_H
#define SELECTSONGDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QListWidget>

namespace Ui {
class SelectSongDialog;
}

class SelectSongDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectSongDialog(const QStringList& availableSongs, QWidget *parent = nullptr);
    ~SelectSongDialog();

    QStringList getSelectedSongs() const;

private:
    Ui::SelectSongDialog *ui;
};

#endif // SELECTSONGDIALOG_H
