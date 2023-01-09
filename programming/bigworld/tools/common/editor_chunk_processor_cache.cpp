#include "pch.hpp"
#include "editor_chunk_processor_cache.hpp"
#include "chunk/chunk_processor_manager.hpp"

BW_BEGIN_NAMESPACE

/**
*	this function is called whenever a processor is finished
*	if we registered as a listener of this processor
*	@param processor the processor which just finished.
*	@param hasUnregistered out,if I have unregistered
*	  myself as listener.
*/
void EditorChunkProcessorCache::onChunkProcessorFinished( 
	ChunkProcessor* pProcessor, 
	bool& hasUnregisteredFromListener )
{
	BW_GUARD;
	MF_ASSERT( pProcessor != NULL );

	pProcessor->delListener( this );
	hasUnregisteredFromListener = true;
	processingChunkProcessors_.delProcessor( pProcessor );
}


/**
*	this function records a ChunkProcessor which is
*	just thrown to background, in case we want to 
*	cancel the under processing processors later,
*	we know who to cancel.
*	@param processor the processor thrown to the background
*/
void EditorChunkProcessorCache::registerProcessor( ChunkProcessorPtr processor )
{
	BW_GUARD;
	processor->addListener( this );
	processingChunkProcessors_.addProcessor( processor );
}

/**
*	Check if we are currently processing.
*	This is just a check of if we have a current processor attached.
*	The current processor may have stopped and is waiting to be cleaned up.
*	@return true if navmesh is being calculated.
*/
bool EditorChunkProcessorCache::isBeingCalculated() const
{
	BW_GUARD;
	return ( currentProcessor_.get() != NULL );
}


/**
*	stop all the ChunkProcessors who have been 
*	generated by me and are still not finished
*	the processing.
*	@param pManager the processor manager.
*	@param outUnfinishedProcessors out, all the
*		current unfinished processors will be appended
*		to it. it will NOT be cleared before appending.
*	@return true on success.
*/
bool EditorChunkProcessorCache::cancelAllCalculating( 
	ChunkProcessorManager* pManager,
	ChunkProcessors& outUnfinishedProcessors )
{
	BW_GUARD;
	ChunkProcessorList& processors = 
		processingChunkProcessors_.chunkProcessors();
	for ( ChunkProcessorList::iterator it = processors.begin();
		it != processors.end(); ++it )
	{
		(*it)->stop();
		outUnfinishedProcessors.addProcessor( (*it) );
	}

	return true;

}
BW_END_NAMESPACE
