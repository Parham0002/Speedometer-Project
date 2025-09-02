#include "window.h"
#include <QApplication>
#ifdef UARTCOM
#include "uartservice.h"
#else
#include "tcpservice.h"
#endif
int main(int argc, char **argv)
{

#ifdef UARTCOM
    UARTService service;
#else
    TCPService service;
#endif

    QApplication app(argc, argv);

    Window window{&service};
    window.show(); // visar dialogfönstret

    return app.exec();
}
