#ifndef USERTHREAD_H
#define USERTHREAD_H

#include <QThread>
#include <QTimer>
#include <QSignalMapper>
#include <QLibrary>

//#define USEREXPERIMENT mykilobotexperiment //user edit
//#include "mykilobotexperiment.h" //user edit
#include "toolchain/kilobotexperiment.h"
#include "kilobotidassignment.h"
#include "kilobotcalibrate.h"

#include "kilobottracker.h"
#include "kilobotoverheadcontroller.h"
#include <QDebug>

// FIXME: Provide more precise class description
/**
 * This class takes care about whole experiment. Loads the experiment components in separate tread, composed the control UI and run the whole experiment.
 */
class UserThread : public QThread {
Q_OBJECT
public:
    explicit UserThread(KilobotTracker *kbtracker, KilobotOverheadController *ohc, QObject *parent = 0) ;
    KilobotExperiment *getExperimentPointer() ;
public slots:

    bool chooseInternalExperiments(experimentType expType, int opt = 0);
    bool loadLibrary(const QString& filename);
    bool exptLoaded();
signals:

    void setLibName(QString);

    void setGUILayout(QWidget *);

protected:

    void connectExpt(int expType);
private:

    KilobotOverheadController *ohc; /**< pointer to an overhead controller class */

    KilobotTracker *kbtracker;      /**< pointer to a kilobot tracker */
    QString currExptFilename;       /**< name of the currently loaded experiment file */
    KilobotExperiment *currExpt;    /**< pointer to a currently loaded experiment */

    experimentType currExptType;    /**< type of currently loaded experiment */

    QTimer timer;

    QSignalMapper *mapper = NULL;   /**< signall mapper class */

    void run();
};

#endif // USERTHREAD_H
