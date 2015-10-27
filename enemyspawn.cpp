#include <QString>
#include <QLabel>
#include <QDebug>
#include <string>
#include <random>

using namespace std;

int random_int(int min, int max) {
    static std::default_random_engine engine { std::random_device{}() };
    std::uniform_int_distribution<int> distro{min, max};
    return distro(engine);
}

void enemyGen()
{
    /*auto label_left = random_int(0, this->geometry().width() - 32);
    auto label_top = random_int(ui->btnStart->geometry().bottom(),
                   this->geometry().height() - 32);

    Enemy *alien = new Enemy(ui->widgetView);
    QPixmap evil(":/images/asteroid.png");
    alien->setPixmap(evil);
    alien->setGeometry(QRect(label_left, label_top, 32, 32));
    alien->setScaledContents(true);
    alien->show(); */
}


