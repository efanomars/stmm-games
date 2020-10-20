/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   private-listenerstk.h
 */

#ifndef STMG_PRIVATE_LISTENER_STK_H
#define STMG_PRIVATE_LISTENER_STK_H

#include <cassert>
#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace stmg
{

using std::shared_ptr;

namespace Private
{

/** Listener stack class.
 * Stores and calls the listeners before and after a certain action.
 * Template param T: (*T) is the listener object the methods of which are called
 * by callPre and callPost which have any signature (except for the return
 * type which is void).
 *
 * This class ensures that
 *  - every Listener's PreXXX function is matched by a PostXXX
 *    (unless the listener was removed in between)
 *  - no PostXXX is called without a PreXXX
 *    (when a listener is added in between)
 *  - PostXXX of listeners is called in reverse order of PreXXX
 *  - multiple addition and removal of the same listener are handled correctly
 *
 * The user of this class has to ensure that listeners are removed
 * before they are destroyed.
 *
 * This is called a listener stack because callbacks can be nested.
 */
// Note: this class could probably use a RAII interface, but only if PreCalled
// objects don't have to reallocate to the heap each time it is used.
template<class T>
class ListenerStk
{
public:
	/** Whether there are no listeners.
	 * @return Whether listener stack is empty.
	 */
	inline bool isEmpty() noexcept
	{
		return m_oListeners.empty();
	}
	/** Add a listener.
	 * If the same listener is added more than once a reference count is increased.
	 * Regardless of the reference count the listener "Pre" and "Post" functions
	 * are called only once.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void addListener(T* p0Listener) noexcept
	{
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::addListener p0Listener=" << (int64_t)p0Listener << '\n';
		assert(p0Listener != nullptr);
		auto it = m_oListeners.find(p0Listener);
		if (it == m_oListeners.end()) {
			m_oListeners.insert(std::make_pair(p0Listener, 1));
		} else {
			const int32_t nCount = it->second;
			assert(nCount > 0);
			it->second = nCount + 1;
		}
		listenerPreCalledDirty(nullptr);
	}
	/** Remove a listener.
	 * Decrements the reference count for the listener, if it becomes 0 the
	 * listener is actually removed.
	 * @see addListener().
	 * @param p0Listener The listener. Must exist. Cannot be null.
	 */
	void removeListener(T* p0Listener) noexcept
	{
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::removeListener p0Listener=" << (int64_t)p0Listener << '\n';
		assert(p0Listener != nullptr);
		auto it = m_oListeners.find(p0Listener);
		#ifndef NDEBUG
		assert(it != m_oListeners.end());
		const int32_t nCount = it->second;
		#else
		// WARNING! COMPILER BUG? Following needed because of Release warning about a null dereference.
		const int32_t nCount = ((it == m_oListeners.end()) ? 0 : it->second);
		#endif //NDEBUG
		//const int32_t nCount = it->second;
		assert(nCount > 0);
		if (nCount == 1) {
			m_oListeners.erase(it);
			listenerPreCalledDirty(p0Listener);
		} else if (nCount > 0) { // Also needed because of warning about a null dereference
			it->second = nCount - 1;
		}
	}
	/** Opaque structure used to match pre and post action calls.
	 */
	struct PreCalled {
	private:
		bool isDirtyReset() noexcept
		{
			if (m_bDirty) {
				m_bDirty = false;
				return true;
			}
			return false;
		}
		bool contains(T* p0Listener) noexcept
		{
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::PreCalled::contains p0Listener=" << (int64_t)p0Listener << '\n';
			return (m_oPreListener.find(p0Listener) != m_oPreListener.end());
		}
		void pushBack(T* p0Listener) noexcept
		{
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::PreCalled::pushBack p0Listener=" << (int64_t)p0Listener << '\n';
			const int32_t nIdx = static_cast<int32_t>(m_aPreListener.size());
			m_aPreListener.push_back(p0Listener);
			//assert(m_oPreListener.find(p0Listener) == m_oPreListener.end());
			m_oPreListener.insert(std::make_pair(p0Listener, nIdx));
		}
		T* popBack() noexcept
		{
			T* p0Listener = nullptr;
			while (!m_aPreListener.empty()) {
				p0Listener = m_aPreListener.back();
				m_aPreListener.pop_back();
				if (p0Listener != nullptr) {
					m_oPreListener.erase(p0Listener);
					break; // while
				}
			}
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::PreCalled::popBack p0Listener=" << (int64_t)p0Listener << '\n';
			return p0Listener;
		}
		void remove(T* p0Listener) noexcept
		{
//std::cout << "ListenerStk<" << typeid(p0Listener).name() << ">::PreCalled::remove p0Listener=" << (int64_t)p0Listener << '\n';
			auto it = m_oPreListener.find(p0Listener);
			if (it != m_oPreListener.end()) {
				const int32_t nIdx = it->second;
				assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aPreListener.size())));
				m_aPreListener[nIdx] = nullptr;
				m_oPreListener.erase(it);
			}
		}
	private:
		friend class ListenerStk<T>;
		// The Listeners in the order their PreXXX was called
		//   If a Listener is removed (ex. on a boabloFreeze) it is set to nullptr here so that
		//   its PostXXX won't be called
		std::vector<T*> m_aPreListener;
		// For "fast" deletion
		std::unordered_map<T*,int32_t> m_oPreListener; // Key: Listener, Value: position in m_aPreListener
		// Adding or removing listeners (to m_oBoabloListeners) sets this to true
		bool m_bDirty = false;
	};
	/** Returns a fresh PreCalled instance.
	 * @return The instance "pointer" (tip: use auto).
	 */
	typename std::list< shared_ptr<PreCalled> >::iterator grabPreCalled() noexcept
	{
//std::cout << "ListenerStk::grabPreCalled" << '\n';
		shared_ptr<PreCalled> refPreCalled;
		if (m_oFreePreCalled.empty()) {
			refPreCalled = shared_ptr<PreCalled>(new PreCalled());
		} else {
			refPreCalled = *m_oFreePreCalled.begin();
			m_oFreePreCalled.pop_front();
		}
		refPreCalled->m_bDirty = false;
		refPreCalled->m_aPreListener.clear();
		refPreCalled->m_oPreListener.clear();
		m_oInUsePreCalled.push_front(refPreCalled);
//std::cout << "                          m_oFreePreCalled.size()=" << m_oFreePreCalled.size() << "  m_oInUsePreCalled.size()=" << m_oInUsePreCalled.size() << '\n';
		return m_oInUsePreCalled.begin();
	}
	/** Frees a PreCalled instance.
	 * @param itPreCalled The instance as returned by grabPreCalled(). Must exist.
	 */
	void freePreCalled(const typename std::list< shared_ptr<PreCalled> >::iterator itPreCalled) noexcept
	{
//std::cout << "ListenerStk::freePreCalled" << '\n';
		const shared_ptr<PreCalled>& refFreePreCalled = *itPreCalled;
		assert(refFreePreCalled->m_oPreListener.size() == 0);
		m_oFreePreCalled.push_front(refFreePreCalled);
		m_oInUsePreCalled.erase(itPreCalled);
//std::cout << "                          m_oFreePreCalled.size()=" << m_oFreePreCalled.size() << "  m_oInUsePreCalled.size()=" << m_oInUsePreCalled.size() << '\n';
	}
	/** Calls the "Pre" function of all the listeners.
	 * @param itPreCalled The instance as returned by grabPreCalled(). Must exist.
	 * @param oFun The function pointer that is called. Must be a method of class T.
	 * @param oParams The parameters passed to the function.
	 */
	template <class Function, typename...Params>
	void callPre(const typename std::list< shared_ptr<PreCalled> >::iterator itPreCalled
						, const Function& oFun, Params&... oParams) noexcept
	{
		auto itListener = m_oListeners.begin();
		while (itListener != m_oListeners.end()) {
			T* p0CurListener = itListener->first;
			assert(p0CurListener != nullptr);
			if (!(*itPreCalled)->contains(p0CurListener)) {
				(*itPreCalled)->pushBack(p0CurListener);
				(p0CurListener->* oFun)(oParams...);
				if ((*itPreCalled)->isDirtyReset()) {
					// new listeners might have been added,
					// the order might have been disrupted by a Listener removal
					itListener = m_oListeners.begin();
				} else {
					++itListener;
				}
			} else {
				++itListener;
			}
		}
	}
	/** Calls the "Post" function of the listeners.
	 * The "Post" function is only called for the listeners that had their "Pre"
	 * function called by callPre() (in reverse order) and weren't removed since.
	 * @param itPreCalled The instance as returned by grabPreCalled(). Must exist.
	 * @param oFun The function pointer that is called. Must be a method of class T.
	 * @param oParams The parameters passed to the function.
	 */
	template <class Function, typename...Params>
	void callPost(const typename std::list< shared_ptr<PreCalled> >::iterator itPreCalled
						, const Function& oFun, Params&...oParams) noexcept
	{
		T* p0Listener = (*itPreCalled)->popBack();
		while (p0Listener != nullptr) {
			(p0Listener->* oFun)(oParams...);
			p0Listener = (*itPreCalled)->popBack();
		}
	}
private:
	// When a Listener is removed this is searched and if found its pointer set to nullptr
	/** Mark the PreCalled stack of objects as dirty.
	 * When a listener is added or removed the dirty bit is set. When a listener is
	 * removed PreCalled is also informed to not call its "Post" function if "Pre" was called.
	 * @param p0RemoveListener The listener to remove or nullptr if a listener was added.
	 */
	void listenerPreCalledDirty(T* p0RemoveListener) noexcept
	{
		for (auto& refPreCalled : m_oInUsePreCalled) {
			assert(refPreCalled);
			refPreCalled->m_bDirty = true;
			if (p0RemoveListener != nullptr) {
				refPreCalled->remove(p0RemoveListener);
			}
		}
	}
	//TODO implement this with a vector<std::pair<T*, int32_t>> because of CPU cache lines
	// contains all the (non owning) Listeners of type T with the number of times they were added
	std::unordered_map< T*, int32_t > m_oListeners;
	// represents the current call stack
	std::list< shared_ptr<PreCalled> > m_oInUsePreCalled;
	// When not in use stored here for recycling
	std::list< shared_ptr<PreCalled> > m_oFreePreCalled;
};

} // namespace Private
} // namespace stmg

#endif	/* STMG_PRIVATE_LISTENER_STK_H */
