/**********************************************************************
 * 
 *  NAME:         ITEMLIST.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          23.01.96 MHM created
 *
 *********************************************************************/

#include "itemlist.h"

int GetUniqueID(void)
{
	static int nCurrentId = 0;

    return nCurrentId++;
};


ItemList::ItemList(void)
{
	nAllocationThreshold = 25;

	Item = NULL;
	nItems = 0;
    nItemsAllocated = 0;
};

ItemList::ItemList(ItemList *pItems)
{
	nAllocationThreshold = 25;

	Item = NULL;
	nItems = 0;
    nItemsAllocated = 0;

    // Allocate array
	Resize(pItems->nItemsAllocated);

    // Copy items
	for(nItems = 0 ; nItems < pItems->nItems ; nItems ++)
    	Item[nItems] = pItems->Item[nItems];
};

ItemList::~ItemList()
{
    if(Item)
		delete Item;
};

void ItemList::AddItem(int nValue)
{
	if(nItems == nItemsAllocated)
		// Allocate more memory
		Resize(nItemsAllocated + nAllocationThreshold);

	// Check if there was enough memory
	if(nItems==nItemsAllocated)  return;

	Item[nItems] = nValue;
	nItems ++;
};

void ItemList::DeleteLastItem()
{
	nItems--;

    // Remove too much memory
	if((nItemsAllocated - nItems)>nAllocationThreshold)
		Resize(nItemsAllocated-nAllocationThreshold);
};

void ItemList::DeleteItem(int nIndex)
{
	SwapItems(nIndex,nItems-1);

    DeleteLastItem();
};

void ItemList::SwapItems(int nIndex1,int nIndex2)
{
	int nSwap     = Item[nIndex1];
	Item[nIndex1] = Item[nIndex2];
	Item[nIndex2] = nSwap;
};

int ItemList::SearchItem(int nValue)
{
	for(int nIndex = 0 ; nIndex < nItems ; nIndex ++ )
		if(Item[nIndex]==nValue)
			return nIndex;
    return -1;
};

void ItemList::Reset(void)
{
	delete Item;
    Item = NULL;

	nItems = 0;
    nItemsAllocated = 0;
};

void ItemList::Resize(int nNewSize)
{
	assert(nNewSize >= nItems); 

	int *NewItem = NULL;

	do {
	 	// Assure that nNewsize is a positive number
		nNewSize = max(nNewSize,0);

		  // If nNewsize is greater than zero, create new array
		if(nNewSize > 0){
			// Create triangle array
			NewItem = new int [nNewSize];
				// Check for errors
			if(!NewItem){
				assert(FALSE);
				break;
				};

			// Copy old array to new one
			for(int nIndex = 0 ; nIndex < min(nNewSize,nItems) ;
					nIndex++ )
				NewItem[nIndex] = Item[nIndex];
			};
		// Assign new triangles
		SWAP(int *,Item,NewItem);
		SWAP(int, nItemsAllocated,nNewSize);
		}
	while(FALSE);

	// Delete triangle array not needed
	if(NewItem)
		delete NewItem;
};


/* End of file ITEMLIST.CPP */
