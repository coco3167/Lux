#pragma once
namespace WorkerSM
{
	enum class Objective
	{
		/// <summary>
		/// The worker is in standby
		/// </summary>
		None,


		/// <summary>
		/// The worker is trying to build a new city tile
		/// </summary>
		BuildCity,


		/// <summary>
		/// The worker is gathering resources for itself
		/// </summary>
		CollectResourceForSelf,


		/// <summary>
		/// The worker is gathering resources for a city
		/// </summary>
		CollectResourceForCity,
	};
}

