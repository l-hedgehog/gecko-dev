/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsFilteredContentIterator.h"
#include "nsIContentIterator.h"
#include "nsComponentManagerUtils.h"
#include "nsIContent.h"
#include "nsString.h"
#include "nsIEnumerator.h"

#include "nsContentUtils.h"

#include "nsIDOMNode.h"

//------------------------------------------------------------
nsFilteredContentIterator::nsFilteredContentIterator(nsITextServicesFilter* aFilter) :
  mFilter(aFilter),
  mDidSkip(false),
  mIsOutOfRange(false),
  mDirection(eDirNotSet)
{
  mIterator = do_CreateInstance("@mozilla.org/content/post-content-iterator;1");
  mPreIterator = do_CreateInstance("@mozilla.org/content/pre-content-iterator;1");
}

//------------------------------------------------------------
nsFilteredContentIterator::~nsFilteredContentIterator()
{
}

//------------------------------------------------------------
NS_IMPL_CYCLE_COLLECTING_ADDREF(nsFilteredContentIterator)
NS_IMPL_CYCLE_COLLECTING_RELEASE(nsFilteredContentIterator)

NS_INTERFACE_MAP_BEGIN(nsFilteredContentIterator)
  NS_INTERFACE_MAP_ENTRY(nsIContentIterator)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIContentIterator)
  NS_INTERFACE_MAP_ENTRIES_CYCLE_COLLECTION(nsFilteredContentIterator)
NS_INTERFACE_MAP_END

NS_IMPL_CYCLE_COLLECTION_5(nsFilteredContentIterator,
                           mCurrentIterator,
                           mIterator,
                           mPreIterator,
                           mFilter,
                           mRange)

//------------------------------------------------------------
nsresult
nsFilteredContentIterator::Init(nsINode* aRoot)
{
  NS_ENSURE_TRUE(mPreIterator, NS_ERROR_FAILURE);
  NS_ENSURE_TRUE(mIterator, NS_ERROR_FAILURE);
  mIsOutOfRange    = false;
  mDirection       = eForward;
  mCurrentIterator = mPreIterator;

  mRange = new nsRange();
  nsCOMPtr<nsIDOMNode> domNode(do_QueryInterface(aRoot));
  if (domNode) {
    mRange->SelectNode(domNode);
  }

  nsresult rv = mPreIterator->Init(mRange);
  NS_ENSURE_SUCCESS(rv, rv);
  return mIterator->Init(mRange);
}

//------------------------------------------------------------
nsresult
nsFilteredContentIterator::Init(nsIDOMRange* aRange)
{
  NS_ENSURE_TRUE(mPreIterator, NS_ERROR_FAILURE);
  NS_ENSURE_TRUE(mIterator, NS_ERROR_FAILURE);
  NS_ENSURE_ARG_POINTER(aRange);
  mIsOutOfRange    = false;
  mDirection       = eForward;
  mCurrentIterator = mPreIterator;

  nsCOMPtr<nsIDOMRange> domRange;
  nsresult rv = aRange->CloneRange(getter_AddRefs(domRange));
  NS_ENSURE_SUCCESS(rv, rv);
  mRange = do_QueryInterface(domRange);

  rv = mPreIterator->Init(domRange);
  NS_ENSURE_SUCCESS(rv, rv);
  return mIterator->Init(domRange);
}

//------------------------------------------------------------
nsresult 
nsFilteredContentIterator::SwitchDirections(bool aChangeToForward)
{
  nsINode *node = mCurrentIterator->GetCurrentNode();

  if (aChangeToForward) {
    mCurrentIterator = mPreIterator;
    mDirection       = eForward;
  } else {
    mCurrentIterator = mIterator;
    mDirection       = eBackward;
  }

  if (node) {
    nsresult rv = mCurrentIterator->PositionAt(node);
    if (NS_FAILED(rv)) {
      mIsOutOfRange = true;
      return rv;
    }
  }
  return NS_OK;
}

//------------------------------------------------------------
void
nsFilteredContentIterator::First()
{
  if (!mCurrentIterator) {
    NS_ERROR("Missing iterator!");

    return;
  }

  // If we are switching directions then
  // we need to switch how we process the nodes
  if (mDirection != eForward) {
    mCurrentIterator = mPreIterator;
    mDirection       = eForward;
    mIsOutOfRange    = false;
  }

  mCurrentIterator->First();

  if (mCurrentIterator->IsDone()) {
    return;
  }

  nsINode *currentNode = mCurrentIterator->GetCurrentNode();
  nsCOMPtr<nsIDOMNode> node(do_QueryInterface(currentNode));

  bool didCross;
  CheckAdvNode(node, didCross, eForward);
}

//------------------------------------------------------------
void
nsFilteredContentIterator::Last()
{
  if (!mCurrentIterator) {
    NS_ERROR("Missing iterator!");

    return;
  }

  // If we are switching directions then
  // we need to switch how we process the nodes
  if (mDirection != eBackward) {
    mCurrentIterator = mIterator;
    mDirection       = eBackward;
    mIsOutOfRange    = false;
  }

  mCurrentIterator->Last();

  if (mCurrentIterator->IsDone()) {
    return;
  }

  nsINode *currentNode = mCurrentIterator->GetCurrentNode();
  nsCOMPtr<nsIDOMNode> node(do_QueryInterface(currentNode));

  bool didCross;
  CheckAdvNode(node, didCross, eBackward);
}

///////////////////////////////////////////////////////////////////////////
// ContentToParentOffset: returns the content node's parent and offset.
//
static void
ContentToParentOffset(nsIContent *aContent, nsIDOMNode **aParent,
                      PRInt32 *aOffset)
{
  if (!aParent || !aOffset)
    return;

  *aParent = nsnull;
  *aOffset  = 0;

  if (!aContent)
    return;

  nsIContent* parent = aContent->GetParent();

  if (!parent)
    return;

  *aOffset = parent->IndexOf(aContent);

  CallQueryInterface(parent, aParent);
}

///////////////////////////////////////////////////////////////////////////
// ContentIsInTraversalRange: returns true if content is visited during
// the traversal of the range in the specified mode.
//
static bool
ContentIsInTraversalRange(nsIContent *aContent,   bool aIsPreMode,
                          nsIDOMNode *aStartNode, PRInt32 aStartOffset,
                          nsIDOMNode *aEndNode,   PRInt32 aEndOffset)
{
  NS_ENSURE_TRUE(aStartNode && aEndNode && aContent, false);

  nsCOMPtr<nsIDOMNode> parentNode;
  PRInt32 indx = 0;

  ContentToParentOffset(aContent, getter_AddRefs(parentNode), &indx);

  NS_ENSURE_TRUE(parentNode, false);

  if (!aIsPreMode)
    ++indx;

  PRInt32 startRes = nsContentUtils::ComparePoints(aStartNode, aStartOffset,
                                                   parentNode, indx);
  PRInt32 endRes = nsContentUtils::ComparePoints(aEndNode, aEndOffset,
                                                 parentNode, indx);
  return (startRes <= 0) && (endRes >= 0);
}

static bool
ContentIsInTraversalRange(nsIDOMRange *aRange, nsIDOMNode* aNextNode, bool aIsPreMode)
{
  nsCOMPtr<nsIContent> content(do_QueryInterface(aNextNode));
  NS_ENSURE_TRUE(content && aRange, false);

  nsCOMPtr<nsIDOMNode> sNode;
  nsCOMPtr<nsIDOMNode> eNode;
  PRInt32 sOffset;
  PRInt32 eOffset;
  aRange->GetStartContainer(getter_AddRefs(sNode));
  aRange->GetStartOffset(&sOffset);
  aRange->GetEndContainer(getter_AddRefs(eNode));
  aRange->GetEndOffset(&eOffset);
  return ContentIsInTraversalRange(content, aIsPreMode, sNode, sOffset, eNode, eOffset);
}

//------------------------------------------------------------
// Helper function to advance to the next or previous node
nsresult 
nsFilteredContentIterator::AdvanceNode(nsIDOMNode* aNode, nsIDOMNode*& aNewNode, eDirectionType aDir)
{
  nsCOMPtr<nsIDOMNode> nextNode;
  if (aDir == eForward) {
    aNode->GetNextSibling(getter_AddRefs(nextNode));
  } else {
    aNode->GetPreviousSibling(getter_AddRefs(nextNode));
  }

  if (nextNode) {
    // If we got here, that means we found the nxt/prv node
    // make sure it is in our DOMRange
    bool intersects = ContentIsInTraversalRange(mRange, nextNode, aDir == eForward);
    if (intersects) {
      aNewNode = nextNode;
      NS_ADDREF(aNewNode);
      return NS_OK;
    }
  } else {
    // The next node was null so we need to walk up the parent(s)
    nsCOMPtr<nsIDOMNode> parent;
    aNode->GetParentNode(getter_AddRefs(parent));
    NS_ASSERTION(parent, "parent can't be NULL");

    // Make sure the parent is in the DOMRange before going further
    bool intersects = ContentIsInTraversalRange(mRange, nextNode, aDir == eForward);
    if (intersects) {
      // Now find the nxt/prv node after/before this node
      nsresult rv = AdvanceNode(parent, aNewNode, aDir);
      if (NS_SUCCEEDED(rv) && aNewNode) {
        return NS_OK;
      }
    }
  }

  // if we get here it pretty much means 
  // we went out of the DOM Range
  mIsOutOfRange = true;

  return NS_ERROR_FAILURE;
}

//------------------------------------------------------------
// Helper function to see if the next/prev node should be skipped
void
nsFilteredContentIterator::CheckAdvNode(nsIDOMNode* aNode, bool& aDidSkip, eDirectionType aDir)
{
  aDidSkip      = false;
  mIsOutOfRange = false;

  if (aNode && mFilter) {
    nsCOMPtr<nsIDOMNode> currentNode = aNode;
    bool skipIt;
    while (1) {
      nsresult rv = mFilter->Skip(aNode, &skipIt);
      if (NS_SUCCEEDED(rv) && skipIt) {
        aDidSkip = true;
        // Get the next/prev node and then 
        // see if we should skip that
        nsCOMPtr<nsIDOMNode> advNode;
        rv = AdvanceNode(aNode, *getter_AddRefs(advNode), aDir);
        if (NS_SUCCEEDED(rv) && advNode) {
          aNode = advNode;
        } else {
          return; // fell out of range
        }
      } else {
        if (aNode != currentNode) {
          nsCOMPtr<nsIContent> content(do_QueryInterface(aNode));
          mCurrentIterator->PositionAt(content);
        }
        return; // found something
      }
    }
  }
}

void
nsFilteredContentIterator::Next()
{
  if (mIsOutOfRange || !mCurrentIterator) {
    NS_ASSERTION(mCurrentIterator, "Missing iterator!");

    return;
  }

  // If we are switching directions then
  // we need to switch how we process the nodes
  if (mDirection != eForward) {
    nsresult rv = SwitchDirections(true);
    if (NS_FAILED(rv)) {
      return;
    }
  }

  mCurrentIterator->Next();

  if (mCurrentIterator->IsDone()) {
    return;
  }

  // If we can't get the current node then 
  // don't check to see if we can skip it
  nsINode *currentNode = mCurrentIterator->GetCurrentNode();

  nsCOMPtr<nsIDOMNode> node(do_QueryInterface(currentNode));
  CheckAdvNode(node, mDidSkip, eForward);
}

void
nsFilteredContentIterator::Prev()
{
  if (mIsOutOfRange || !mCurrentIterator) {
    NS_ASSERTION(mCurrentIterator, "Missing iterator!");

    return;
  }

  // If we are switching directions then
  // we need to switch how we process the nodes
  if (mDirection != eBackward) {
    nsresult rv = SwitchDirections(false);
    if (NS_FAILED(rv)) {
      return;
    }
  }

  mCurrentIterator->Prev();

  if (mCurrentIterator->IsDone()) {
    return;
  }

  // If we can't get the current node then 
  // don't check to see if we can skip it
  nsINode *currentNode = mCurrentIterator->GetCurrentNode();

  nsCOMPtr<nsIDOMNode> node(do_QueryInterface(currentNode));
  CheckAdvNode(node, mDidSkip, eBackward);
}

nsINode *
nsFilteredContentIterator::GetCurrentNode()
{
  if (mIsOutOfRange || !mCurrentIterator) {
    return nsnull;
  }

  return mCurrentIterator->GetCurrentNode();
}

bool
nsFilteredContentIterator::IsDone()
{
  if (mIsOutOfRange || !mCurrentIterator) {
    return true;
  }

  return mCurrentIterator->IsDone();
}

nsresult
nsFilteredContentIterator::PositionAt(nsINode* aCurNode)
{
  NS_ENSURE_TRUE(mCurrentIterator, NS_ERROR_FAILURE);
  mIsOutOfRange = false;
  return mCurrentIterator->PositionAt(aCurNode);
}
