/*  Pcsx2 - Pc Ps2 Emulator
 *  Copyright (C) 2002-2008  Pcsx2 Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "PrecompiledHeader.h"

#include "Threading.h"

namespace Threading
{
	Thread::Thread() :
		m_thread()
	,	m_returncode( 0 )
	,	m_terminated( false )
	,	m_sigterm( 0 )
	,	m_wait_event()
	{
		if( pthread_create( &m_thread, NULL, _internal_callback, this ) != 0 )
			throw Exception::ThreadCreationError();
	}

	Thread::~Thread()
	{
		Close();
	}

	void Thread::Close()
	{
		AtomicExchange( m_sigterm, 1 );
		m_wait_event.Set();
		pthread_join( m_thread, NULL );
	}

	int Thread::GetReturnCode() const
	{
		if( !m_terminated ) 
			throw std::logic_error( "Thread is still running. No return code is available." );

		return m_returncode;
	}

	WaitEvent::WaitEvent() 
	{
		int err = 0;
		
		err = pthread_cond_init(&cond, NULL);
		err = pthread_mutex_init(&mutex, NULL);
	}

	WaitEvent::~WaitEvent()
	{
		pthread_cond_destroy( &cond );
		pthread_mutex_destroy( &mutex );
	}

	void WaitEvent::Set()
	{
		pthread_mutex_lock( &mutex );
		pthread_cond_signal( &cond );
		pthread_mutex_unlock( &mutex );
	}

	void WaitEvent::Wait()
	{
		pthread_mutex_lock( &mutex );
		pthread_cond_wait( &cond, &mutex );
		pthread_mutex_unlock( &mutex );
	}

	MutexLock::MutexLock()
	{
		int err = 0;
		err = pthread_mutex_init( &mutex, NULL );
	}

	MutexLock::~MutexLock()
	{
		pthread_mutex_destroy( &mutex );
	}

	void MutexLock::Lock()
	{
		pthread_mutex_lock( &mutex );
	}

	void MutexLock::Unlock()
	{
		pthread_mutex_unlock( &mutex );
	}

	//////////////////////////////////////////////////////////////////////
	// define some overloads for InterlockedExchanges
	// for commonly used types, like u32 and s32.

	__forceinline void AtomicExchange( u32& Target, u32 value )
	{
		pcsx2_InterlockedExchange( (volatile long*)&Target, value );
	}

	__forceinline void AtomicExchangeAdd( u32& Target, u32 value )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, value );
	}

	__forceinline void AtomicIncrement( u32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, 1 );
	}

	__forceinline void AtomicDecrement( u32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, -1 );
	}

	__forceinline void AtomicExchange( s32& Target, s32 value )
	{
		pcsx2_InterlockedExchange( (volatile long*)&Target, value );
	}

	__forceinline void AtomicExchangeAdd( s32& Target, u32 value )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, value );
	}

	__forceinline void AtomicIncrement( s32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, 1 );
	}

	__forceinline void AtomicDecrement( s32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, -1 );
	}

	__forceinline void _AtomicExchangePointer( const void ** target, const void* value )
	{
		pcsx2_InterlockedExchange( (volatile long*)target, (long)value );
	}

	__forceinline void _AtomicCompareExchangePointer( const void ** target, const void* value, const void* comparand )
	{
		pcsx2_InterlockedCompareExchange( (volatile long*)target, (long)value, (long)comparand );
	}

}