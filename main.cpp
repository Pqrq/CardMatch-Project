#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <random>
#include <QFile>
#include <QIcon>
#include <QResource>

#define maxTries 50 // Maximum number of tries
#define originalArray {QString::fromStdString("Sans"),QString::fromStdString("Sans"),QString::fromStdString("Papyrus"),QString::fromStdString("Papyrus"),QString::fromStdString("Frisk"),QString::fromStdString("Frisk"),QString::fromStdString("Mettaton"),QString::fromStdString("Mettaton"),QString::fromStdString("Flowey"),QString::fromStdString("Flowey"),QString::fromStdString("Chara"),QString::fromStdString("Chara"),QString::fromStdString("Toriel"),QString::fromStdString("Toriel"),QString::fromStdString("Reimu"),QString::fromStdString("Reimu"),QString::fromStdString("Marisa"),QString::fromStdString("Marisa"),QString::fromStdString("Yukari"),QString::fromStdString("Yukari"),QString::fromStdString("Sanae"),QString::fromStdString("Sanae"),QString::fromStdString("Cirno"),QString::fromStdString("Cirno"),QString::fromStdString("Remilla"),QString::fromStdString("Remilla"),QString::fromStdString("Flandre"),QString::fromStdString("Flandre"),QString::fromStdString("Sakuya"),QString::fromStdString("Sakuya")}

std::array<QString, 30> shuffledArray; // Shuffled array of character names
QPushButton *cardsTable[5][6]; // Array of buttons (except the restart button)
int numberOfRemainingTries; // Number of remaining tries of the current game.
int score; // Score of the current game
QLabel *topRow[4]; // Array of top row labels (note that restart button is not included here)
QLabel *middleRow; // Middle row label (which is just an empty label)
QPushButton *restartButton; // Restart button of the game
int fittx; // x-coordinate of the first opened card, which is held before pushing the second button and comparing their strings
int fitty; // y-coordinate of the first opened card, which is held before pushing the second button and comparing their strings
bool wasEnabled[5][6]; // Array to store the state of buttons. By saying "states", we indicate whether the button is enabled or not.

// Returns a shuffled version of a predefined constant array
// (which is the array of our character names, which are Sans,Mettaton,Remilla,... etc.).
std::array<QString, 30> nameArrayShuffler() {
    std::array<QString, 30> shuffledArray = originalArray;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffledArray.begin(), shuffledArray.end(), gen);
    return shuffledArray;
}

// Disables all of the buttons except the restart button
void disconnectAll() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            cardsTable[i][j]->blockSignals(true);
        }
    }
}

// Enables the buttons which have been enabled before using disconnectAll()
// (except the restart button).
void freeButtons() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            if (wasEnabled[i][j]) {
                cardsTable[i][j]->blockSignals(false);
            }
        }
    }
    restartButton->blockSignals(false);
}

// This will be called if the two pressed buttons' corresponding character
// names are same. This function temporarily disconnects the matched
// cards' buttons and makes them invisible (until restarting the game) and
// changes other variables accordingly.
void sameButtonFunc(int x, int y) {
    wasEnabled[x][y] = false;
    cardsTable[x][y]->blockSignals(true);
    cardsTable[fittx][fitty]->setText("");
    cardsTable[x][y]->setText("");
    cardsTable[fittx][fitty]->setStyleSheet("QPushButton {background-color: #1a1a1a;border-width: 2px;border-color: #1a1a1a;border-style: solid;}");
    cardsTable[x][y]->setStyleSheet("QPushButton {background-color: #1a1a1a;border-width: 2px;border-color: #1a1a1a;border-style: solid;}");
    fittx = -1;
    fitty = -1;
    freeButtons();
}

// This will be called if the two pressed buttons' corresponding character
// names are different. In this case, we disconnect all buttons for 1 second,
// then we re-connect the non-matched cards' buttons and restart button,
// change the appropriate global variables and continue the game.
void diffButtonFunc(int x, int y) {
    wasEnabled[fittx][fitty] = true;
    cardsTable[fittx][fitty]->setText("?");
    cardsTable[x][y]->setText("?");
    fittx = -1;
    fitty = -1;
    freeButtons();
}

// Restarts the game. In other terms, it initializes the global variables,
// re-shuffles the buttons' corresponding character names and starts a new game.
void restarter() {
    fittx = -1;
    fitty = -1;
    score = 0;
    numberOfRemainingTries = maxTries;
    topRow[3]->setText(QString::number(maxTries)); // Reset number of tries
    topRow[1]->setText(QString::number(0)); // Reset score
    shuffledArray = nameArrayShuffler(); // Reshuffle deck

    // Reset buttons
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            wasEnabled[i][j] = true;
            cardsTable[i][j]->setStyleSheet("QPushButton {background-color: #ff3333;border-width: 2px;}");
            cardsTable[i][j]->blockSignals(false);
            cardsTable[i][j]->setText("?");
        }
    }
}

// Sets up the application
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QResource::registerResource(":/resources.qrc"); // Initializes resources

    // Initializes style
    QFile styleFile(":/resources/SyNet.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    // Initializes icon
    QIcon icon(":/resources/icon.png");
    app.setWindowIcon(icon);

    // Initializes labels
    topRow[0] = new QLabel("Score");
    topRow[1] = new QLabel("0");
    topRow[2] = new QLabel("No. of Tries Remaining");
    topRow[3] = new QLabel(QString::number(maxTries));
    middleRow = new QLabel("");

    // Initializes restart button
    restartButton = new QPushButton("Restart");
    QObject::connect(restartButton, &QPushButton::clicked, []() {restarter();});

    // Create a main window
    QWidget window;
    window.setWindowTitle("Card Match Game");

    // Create a main layout for the window
    QVBoxLayout* mainLayout = new QVBoxLayout(&window);

    // Create the first grid layout
    QGridLayout* gridLayout1 = new QGridLayout();

    for (int i = 0; i < 5; i++) {
        if (i != 4) {
            gridLayout1->addWidget(topRow[i], 0, i);
            topRow[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            topRow[i]->setMaximumHeight(90);
        } else {
            gridLayout1->addWidget(restartButton, 0, 4);
            restartButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            restartButton->setMaximumHeight(90);
        }
        gridLayout1->setColumnStretch(i, 2);
    }
    gridLayout1->setRowStretch(0, 1);

    // Create the second grid layout
    QGridLayout* gridLayout2 = new QGridLayout();
    gridLayout2->addWidget(middleRow, 0, 0);
    gridLayout2->setColumnStretch(0, 1);
    gridLayout2->setRowStretch(0, 16);
    middleRow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    middleRow->setMaximumHeight(75);

    // Create the third grid layout
    QGridLayout* gridLayout3 = new QGridLayout();

    // Initializing the buttons
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 6; y++) {
            cardsTable[x][y] = new QPushButton("?");
            QObject::connect(cardsTable[x][y], &QPushButton::clicked,&app, [&, x, y]() {
                // General function for all buttons
                if (fittx == -1) {
                    // If there was no button pressed in this turn
                    cardsTable[x][y]->setText(shuffledArray[6 * x + y]);
                    wasEnabled[x][y] = false;
                    cardsTable[x][y]->blockSignals(true);
                    fittx = x;
                    fitty = y;
                    return;
                } else {
                    // If there was a button pressed in this turn
                    cardsTable[x][y]->setText(shuffledArray[6 * x + y]);
                    numberOfRemainingTries--;
                    topRow[3]->setText(QString::number(numberOfRemainingTries));
                    QTimer *timer = new QTimer();
                    // Compares the first opened card with the second
                    if (shuffledArray[6 * x + y].compare(shuffledArray[6 * fittx + fitty]) == 0) {
                        score++;
                        topRow[1]->setText(QString::number(score));
                        if (score == 15) {
                            sameButtonFunc(x, y);
                        } else {
                            QObject::connect(timer, &QTimer::timeout, [&, x, y]() {
                                sameButtonFunc(x, y); // Eliminate the same buttons
                            });
                        }
                    } else {
                        if (numberOfRemainingTries > 0) {
                            QObject::connect(timer, &QTimer::timeout, [&, x, y]() {
                                diffButtonFunc(x, y); // Close the buttons
                            });
                        } else {
                            diffButtonFunc(x, y);
                        }
                    }
                    timer->setInterval(1000);
                    timer->setSingleShot(true);
                    timer->start();
                    disconnectAll();
                    restartButton->blockSignals(true);
                }
                // Checks if the user has won
                if (score == 15) {
                    restartButton->blockSignals(false);
                    QMessageBox successBox;
                    successBox.setText("Congratulations brother!");
                    successBox.setWindowTitle("Success");
                    successBox.setStandardButtons(QMessageBox::Cancel);
                    successBox.exec();
                }
                // Checks if the user has failed
                if ((numberOfRemainingTries == 0) && (score < 15)) {
                    for (int i = 0; i < 5; i++) {
                        for (int j = 0; j < 6; j++) {
                            if (wasEnabled[i][j]) {
                                cardsTable[i][j]->setText(shuffledArray[6 * i + j]);
                            }
                        }
                    }
                    restartButton->blockSignals(false);
                    QMessageBox failBox;
                    failBox.setText("Mission falied!");
                    failBox.setWindowTitle("Fail");
                    failBox.setStandardButtons(QMessageBox::Cancel);
                    failBox.exec();
                }
            });
        }
    }

    // Filling the tables with the buttons above, while arranging their size policies
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            gridLayout3->addWidget(cardsTable[i][j], i, j);
            cardsTable[i][j]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        }
        gridLayout3->setRowStretch(i, 1);
    }
    for (int j = 0; j < 6; j++) {
        gridLayout3->setColumnStretch(j, 2);
    }

    // Add the grid layouts to the main layout
    mainLayout->addLayout(gridLayout1);
    mainLayout->addLayout(gridLayout2);
    mainLayout->addLayout(gridLayout3);

    // Initially the game begins
    restarter();

    // Show the main window
    window.show();

    return app.exec();
}
