#include "userthread.h"

/**
 * Default constructor
 * @param kbtracker pointer to a tracker, which is used for tracking the kilobots
 * @param ohc pointer to a overhead communication class
 * @param parent Object parent
 */
//TODO: Check if the parent is necessery or if it can be removed
UserThread::UserThread(KilobotTracker *kbtracker, KilobotOverheadController *ohc, QObject *parent) {

    Q_UNUSED(parent)

    this->ohc = ohc;
    this->kbtracker = kbtracker;

    // set the kilobot ID assignment as current experiment
    this->currExpt = new KilobotIDAssignment(BINARY);
    this->connectExpt(ID_ASSIGNMENT);

}

/**
 * Load the experiment in form of dynamic linked library based on the file name.
 * @param filename Absolute or relative path to the library
 * @return true if experiment correctly loaded, false otherwise
 */
bool UserThread::loadLibrary(const QString& filename) {
    bool success = false;

    // load library
    QLibrary library(filename);

    // resolve access to class
    typedef KilobotExperiment *(*createExperimentFunction)();
    createExperimentFunction createExperiment = (createExperimentFunction) library.resolve("createExpt");

    if (createExperiment) {

        if (currExptFilename != filename) {
            currExptFilename = filename;
        }

        if (exptLoaded()) this->currExpt->deleteLater();
        this->currExpt = createExperiment();
        emit setLibName(filename);
        // set as current experiment for now
        this->connectExpt(USER_EXP);

        // setup GUI
        emit setGUILayout(this->currExpt->createGUI());
        success = true;

    } else {
        emit setLibName(QString("<load failed: ") + library.errorString() + QString(">"));
        this->currExpt = NULL;
    }
    return success;
}

// TODO: replace the opt parameter by enum or provide more precise description

/** Load some predefined experiment
 *
 * @param expType Type of the experiment
 * @param opt Set some options for the experiment
 * @return true in case of correct experiment initialization, false otherwise
 */
bool UserThread::chooseInternalExperiments(experimentType expType, int opt) {
    bool success = false;
    this->currExptFilename = "";
    switch (expType) {
        case ID_ASSIGNMENT:
            if (opt == 2) {
                if (exptLoaded()) this->currExpt->deleteLater();
                this->currExpt = new KilobotIDAssignment(BINARY);
            }
            if (opt == 3) {
                if (exptLoaded()) this->currExpt->deleteLater();
                this->currExpt = new KilobotIDAssignment(BASETHREE);
            }
            success = true;
            break;
        case CALIBRATION:
            if (exptLoaded()) this->currExpt->deleteLater();
            this->currExpt = new KilobotCalibrate((this->kbtracker->kbMinSize + this->kbtracker->kbMaxSize) / 2.0);
            //this->currExpt = new KilobotCalibrate( this->kbtracker->kbMaxSize );
            success = true;
            break;
        default:
            break;
    }
    this->connectExpt(expType);
    return success;
}

//TODO: Check if expType can be replaced from this function - seems to correctly set in parent functions
// Maybe it can be switch to a different data type (experimentType)

/**
 *  Create connection between slots and signals of newly loaded library.
 *  Signals and slots are used as the only one communication chanel between experiment and the application to provide safe-thread function.
 * @param expType Type of the experiment
 */
void UserThread::connectExpt(int expType) {
    this->currExptType = (experimentType) expType;

    // join signals / slots

    // expt -> tracker
    connect(this->currExpt, SIGNAL(updateKilobotStates()), kbtracker, SLOT(updateKilobotStates()));
    connect(this->currExpt, SIGNAL(getInitialKilobotStates()), kbtracker, SLOT(getInitialKilobotStates()));
    connect(this->currExpt, SIGNAL(setTrackingType(int)), kbtracker, SLOT(setTrackingType(int)));
    connect(this->currExpt, SIGNAL(sendBroadcastingState(bool)), kbtracker,
            SLOT(updateExperimentBroadcastingState(bool)));

    if (mapper != NULL) mapper->deleteLater();
    mapper = new QSignalMapper(this);
    mapper->setMapping(this->currExpt, this->currExptType);
    connect(this->currExpt, SIGNAL(experimentComplete()), mapper, SLOT(map()));
    connect(mapper, SIGNAL(mapped(int)), kbtracker, SLOT(LOOPstartstop(int)));

    // drawing signal / slots
    connect(this->currExpt, SIGNAL(drawCircle(QPointF, float, QColor, int, std::string, bool)), this->kbtracker,
            SLOT(drawCircle(QPointF, float, QColor, int, std::string, bool)));
    connect(this->currExpt, SIGNAL(drawLine(std::vector<cv::Point>, QColor, int, std::string, bool)), this->kbtracker,
            SLOT(drawLine(std::vector<cv::Point>, QColor, int, std::string, bool)));
    connect(this->currExpt, SIGNAL(clearDrawings()), this->kbtracker, SLOT(clearDrawings()));
    connect(this->currExpt, SIGNAL(drawCircleOnRecordedImage(QPointF, float, QColor, int, std::string)),
            this->kbtracker, SLOT(drawCircleOnRecordedImage(QPointF, float, QColor, int, std::string)));
    connect(this->currExpt, SIGNAL(clearDrawingsOnRecordedImage()), this->kbtracker,
            SLOT(clearDrawingsOnRecordedImage()));

    // save image/video
    connect(this->currExpt, SIGNAL(saveImage(QString)), this->kbtracker, SLOT(saveImage(QString)));
    connect(this->currExpt, SIGNAL(saveVideoFrames(QString, unsigned int)), this->kbtracker,
            SLOT(saveVideoFrames(QString, unsigned int)));


    // clock for experiment
    this->timer.setInterval(this->currExpt->serviceInterval);
    connect(&this->timer, SIGNAL(timeout()), this->currExpt, SLOT(run()));

    // tracker outputs
    connect(kbtracker, SIGNAL(startExperiment(bool)), this->currExpt, SLOT(initialise(bool)));
    connect(kbtracker, SIGNAL(startExperiment(bool)), &this->timer, SLOT(start()));
    connect(kbtracker, SIGNAL(stopExperiment()), this->currExpt, SLOT(stopExperiment()));
    connect(kbtracker, SIGNAL(stopExperiment()), &this->timer, SLOT(stop()));
    connect(kbtracker, SIGNAL(setRuntimeIdentificationLock(bool)), this->currExpt,
            SLOT(setRuntimeIdentificationLock(bool)));

    // ohc
    connect(this->currExpt, SIGNAL(broadcastMessage(kilobot_broadcast)), this->ohc,
            SLOT(broadcastMessage(kilobot_broadcast)));
    connect(this->currExpt, SIGNAL(signalKilobot(kilobot_message)), this->ohc, SLOT(signalKilobot(kilobot_message)));
    connect(this->ohc, SIGNAL(SignalMsgsQueueState(bool)), this->currExpt, SLOT(GetMsgsQueueState(bool)));

    this->kbtracker->expt = this->currExpt;

}

// TODO: Check if this function is called anyhere and/or is used
void UserThread::run() {
    QThread::run();
}

/**
 * @brief Getter for a pointer to the experiment
 * @return pointer to a current experiment
 */
KilobotExperiment *UserThread::getExperimentPointer() {
    return currExpt;
}

/**
 * @brief Check if the experiment is loaded
 * @return tru if some experiment is loaded
 */
bool UserThread::exptLoaded() {
    return this->currExpt != NULL;
}
