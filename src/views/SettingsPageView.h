#ifndef SETTINGSPAGEVIEW_H
#define SETTINGSPAGEVIEW_H

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class SettingsPageView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPageView(QWidget *parent = nullptr);
    ~SettingsPageView();

    bool isFullscreenChecked() const;
    void setFullscreenChecked(bool checked);

signals:
    void backClicked();
    void fullscreenToggled(bool checked);

private slots:
    void onFullscreenCheckBoxStateChanged(int state);

private:
    QLabel *titleLabel;
    QCheckBox *fullscreenCheckBox;
    QPushButton *backButton;
    QVBoxLayout *mainLayout;
};

#endif // SETTINGSPAGEVIEW_H
