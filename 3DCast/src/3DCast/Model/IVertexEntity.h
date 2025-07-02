#pragma once

namespace Cast
{
	struct IVertexEntity
	{
		virtual ~IVertexEntity() = default;

		virtual void RetransferToBatchMemory() = 0;

		// TODO: Swapping
	};
}
