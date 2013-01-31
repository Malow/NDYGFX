#pragma once

template<typename T>
T& Min( T& A, T& B )
{
	return ( A < B? A : B );
}

template<typename T>
T& Max( T& A, T& B )
{
	return ( A > B? A : B );
}