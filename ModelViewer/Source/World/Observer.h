#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <set>

class Event;
class Observer;
class Observed;


class Event
{
public:
	virtual ~Event() {}
};


class Observer
{
protected:
	virtual void OnEvent( Event* ) {};

	friend class Observed;
};


class Observed
{
	std::set<Observer*> _observers;
public:
	inline void AddObserver( Observer* observer ) { if ( observer ) _observers.insert(observer); }
	inline void RemoveObserver( Observer* observer ) { if ( observer ) _observers.erase(observer); }
protected:
	Observed( Observer* default=0 ) { AddObserver(default); }
	inline void NotifyObservers( Event* e ) { for( std::set<Observer*>::iterator i=_observers.begin(); i != _observers.end(); ++i ) { (*i)->OnEvent(e); } }
};

#endif // _OBSERVER_HPP_