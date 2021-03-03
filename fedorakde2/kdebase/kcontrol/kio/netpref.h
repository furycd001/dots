#ifndef KIOPREFERENCES_H
#define KIOPREFERENCES_H

#include <kcmodule.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QSpinBox;

class KIOPreferences : public KCModule
{
    Q_OBJECT

public:
    KIOPreferences( QWidget* parent = 0, const char* name = 0 );
    ~KIOPreferences();

    void load();
    void save();
    void defaults();
    QString quickHelp() const;

protected slots:
    void proxyConnectTimeoutChanged( int );
    void connectTimeoutChanged( int );
    void responseTimeoutChanged( int );
    void readTimeoutChanged( int );

protected:
    void changed ( bool );

private:
    bool d_valueChanged;

    int d_proxyConnectTimeout;
    int d_connectTimeout;
    int d_responseTimeout;
    int d_readTimeout;

    QGroupBox* d_timeout;
    QSpinBox* d_socketRead;
    QSpinBox* d_proxyConnect;
    QSpinBox* d_serverConnect;
    QSpinBox* d_serverResponse;
};

#endif
