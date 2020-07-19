//
// Created by alfystar on 19/07/20.
//

#include "NCS.h"

using namespace NCS;

Queue *NCS::queueObj;
Accept *NCS::acceptObj;
std::mutex NCS::queueLock;
std::mutex NCS::acceptock;


Queue *NCS::ncsGetQueue (){
	const std::lock_guard<std::mutex> lock(queueLock);
	if(queueObj) //Oggetto già definito
		return queueObj;

	queueObj = new Queue();
	acceptObj = new Accept(queueObj);
	return queueObj;
}


Accept *NCS::ncsGetAccept (){
	const std::lock_guard<std::mutex> lock(acceptock);
	if(acceptObj) //Oggetto già definito
		return acceptObj;

	acceptObj = new Accept(queueObj);
	return acceptObj;
}
