#ifndef RUNWORKERTHREAD_H_
#define RUNWORKERTHREAD_H_

#include <QThread>

#include "Runner.hpp"

namespace uipf {

//A class that does the background work in a thread.
//By deriving QThread QT handles signals/slot thread marshaling automatically.
//for this all Types used in signals have to be registered by qRegisterMetaType() -> see GUIEventDispatcher.cpp
class RunWorkerThread: public QThread {

	Q_OBJECT

public:
	RunWorkerThread(ProcessingChain&, ModuleLoader&);
	virtual ~RunWorkerThread();

	//run module chain in a separate thread
	void run() Q_DECL_OVERRIDE;
	//tell modules to stop work now
	void stop();

private:

	Runner runner_;

};

} // namespace

#endif /* RUNWORKERTHREAD_H_ */
