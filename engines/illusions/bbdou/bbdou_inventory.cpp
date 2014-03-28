/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "illusions/illusions.h"
#include "illusions/bbdou/bbdou_inventory.h"
#include "illusions/bbdou/bbdou_cursor.h"
#include "illusions/bbdou/bbdou_specialcode.h"
#include "illusions/actor.h"
#include "illusions/camera.h"
#include "illusions/dictionary.h"
#include "illusions/input.h"

namespace Illusions {

// InventoryItem

InventoryItem::InventoryItem(uint32 objectId, uint32 sequenceId)
	: _objectId(objectId), _sequenceId(sequenceId),
	_assigned(false), _flag(false), _timesPresent(0), _fieldE(0) {
}

// InventorySlot

InventorySlot::InventorySlot(uint32 namedPointId)
	: _namedPointId(namedPointId), _objectId(0), _inventoryItem(0) {
}

// InventoryBag

InventoryBag::InventoryBag(IllusionsEngine *vm, uint32 sceneId)
	: _vm(vm), _sceneId(sceneId), _isActive(false), _fieldA(0) {
}

void InventoryBag::registerInventorySlot(uint32 namedPointId) {
	_inventorySlots.push_back(new InventorySlot(namedPointId));
}

bool InventoryBag::addInventoryItem(InventoryItem *inventoryItem, InventorySlot *inventorySlot) {
	// NOTE Skipped support for multiple items per slot, not used in BBDOU
	if (!inventorySlot) {
		for (InventorySlotsIterator it = _inventorySlots.begin(); it != _inventorySlots.end(); ++it)
			if (!(*it)->_inventoryItem) {
				inventorySlot = *it;
				break;
			}
	}
	if (inventorySlot) {
		inventorySlot->_inventoryItem = inventoryItem;
		return true;
	}
	return false;
}

void InventoryBag::removeInventoryItem(InventoryItem *inventoryItem) {
	// TODO
}

void InventoryBag::buildItems() {
	for (InventorySlotsIterator it = _inventorySlots.begin();
		it != _inventorySlots.end(); ++it) {
		InventorySlot *inventorySlot = *it;
		InventoryItem *inventoryItem = inventorySlot->_inventoryItem;
		if (inventoryItem) {
			++inventoryItem->_timesPresent;
			if (!inventoryItem->_assigned || inventoryItem->_flag || inventoryItem->_timesPresent > 1)
				inventorySlot->_inventoryItem = 0;
		}
	}
}

// BbdouInventory

BbdouInventory::BbdouInventory(IllusionsEngine *vm, BbdouSpecialCode *bbdou)
	: _vm(vm), _bbdou(bbdou) {
}
				                                             
void BbdouInventory::registerInventoryBag(uint32 sceneId) {
	_inventoryBags.push_back(new InventoryBag(_vm, sceneId));
	_activeBagSceneId = sceneId;
}

void BbdouInventory::registerInventoryItem(uint32 objectId, uint32 sequenceId) {
	_activeBagSceneId = 0;
	_inventoryItems.push_back(new InventoryItem(objectId, sequenceId));
}

void BbdouInventory::registerInventorySlot(uint32 namedPointId) {
	InventoryBag *inventoryBag = getInventoryBag(_activeBagSceneId);
	inventoryBag->registerInventorySlot(namedPointId);
}

void BbdouInventory::addInventoryItem(uint32 objectId) {
	_activeBagSceneId = 0;
	InventoryItem *inventoryItem = getInventoryItem(objectId);
	bool assigned = inventoryItem->_assigned;
	inventoryItem->_assigned = true;
	if (!assigned && !inventoryItem->_flag) {
		for (uint i = 0; i < _inventoryBags.size(); ++i)
			if (!_inventoryBags[i]->addInventoryItem(inventoryItem, 0))
				inventoryItem->_assigned = false;
	}
	if (_activeInventorySceneId)
		refresh();
}

void BbdouInventory::removeInventoryItem(uint32 objectId) {
	InventoryItem *inventoryItem = getInventoryItem(objectId);
	bool flag = inventoryItem->_flag;
	inventoryItem->_flag = true;
	if (!flag && inventoryItem->_assigned) {
		if (_activeInventorySceneId) {
			InventoryBag *inventoryBag = getInventoryBag(_activeInventorySceneId);
			inventoryBag->removeInventoryItem(inventoryItem);
		}
		refresh();
	}
}

void BbdouInventory::open() {
	_activeBagSceneId = 0;
	InventoryBag *inventoryBag = getInventoryBag(_vm->getCurrentScene());
	buildItems(inventoryBag);
	if (_activeInventorySceneId) {
		refresh();
		refresh();
	} else {
		_activeInventorySceneId = _vm->getCurrentScene();
		_index = 1;
		inventoryBag->_isActive = true;
		for (InventoryBag::InventorySlotsIterator it = inventoryBag->_inventorySlots.begin();
			it != inventoryBag->_inventorySlots.end(); ++it) {
			InventorySlot *inventorySlot = *it;
			Common::Point slotPos = _vm->getNamedPointPosition(inventorySlot->_namedPointId);
			Control *control = _vm->_dict->getObjectControl(inventorySlot->_objectId);
			if (control) {
				control->setActorPosition(slotPos);
				control->startSequenceActor(0x0006005A, 2, 0);
			} else {
				inventorySlot->_objectId = _vm->_controls->newTempObjectId();
				_vm->_controls->placeActor(0x00050012, slotPos, 0x0006005A, inventorySlot->_objectId, 0);
			}
			// TODO causeDeclare(0x1B0002, 0, inventorySlot->_objectId, Inventory_cause0x1B0002);
			// TODO causeDeclare(0x1B0001, 0, inventorySlot->_objectId, Inventory_cause0x1B0001);
			// TODO causeDeclare(0x1B0008, 0, inventorySlot->_objectId, Inventory_cause0x1B0001);
		}
		refresh();
	}
}

void BbdouInventory::close() {
	if (!_activeInventorySceneId)
		return;
	InventoryBag *inventoryBag = getInventoryBag(_vm->getCurrentScene());
	for (InventoryBag::InventorySlotsIterator it = inventoryBag->_inventorySlots.begin();
		it != inventoryBag->_inventorySlots.end(); ++it) {
		InventorySlot *inventorySlot = *it;
		Control *control = _vm->_dict->getObjectControl(inventorySlot->_objectId);
		control->startSequenceActor(0x00060187, 2, 0);
    }
    inventoryBag->_isActive = false;
    _activeInventorySceneId = 0;
}

InventoryBag *BbdouInventory::getInventoryBag(uint32 sceneId) {
	for (uint i = 0; i < _inventoryBags.size(); ++i)
		if (_inventoryBags[i]->_sceneId == sceneId)
			return _inventoryBags[i];
	return 0;
}

InventoryItem *BbdouInventory::getInventoryItem(uint32 objectId) {
	for (uint i = 0; i < _inventoryItems.size(); ++i)
		if (_inventoryItems[i]->_objectId == objectId)
			return _inventoryItems[i];
	return 0;
}

void BbdouInventory::refresh() {
	if (!_activeInventorySceneId)
		return;
	InventoryBag *inventoryBag = getInventoryBag(_activeInventorySceneId);
	for (InventoryBag::InventorySlotsIterator it = inventoryBag->_inventorySlots.begin();
		it != inventoryBag->_inventorySlots.end(); ++it) {
		InventorySlot *inventorySlot = *it;
		Control *control = _vm->_dict->getObjectControl(inventorySlot->_objectId);
		InventoryItem *inventoryItem = inventorySlot->_inventoryItem;
		if (inventoryItem) {
			control->startSequenceActor(inventoryItem->_sequenceId, 2, 0);
			control->appearActor();
		} else {
			control->startSequenceActor(0x00060187, 2, 0);
		}
	}
}

void BbdouInventory::buildItems(InventoryBag *inventoryBag) {
	for (InventoryItemsIterator it = _inventoryItems.begin(); it != _inventoryItems.end(); ++it)
		(*it)->_timesPresent = 0;
	inventoryBag->buildItems();
	for (InventoryItemsIterator it = _inventoryItems.begin(); it != _inventoryItems.end(); ++it) {
		InventoryItem *inventoryItem = *it;
	    if (inventoryItem->_assigned && !inventoryItem->_flag &&
			inventoryItem->_timesPresent == 0 &&
			inventoryItem->_objectId != _bbdou->_cursor->_data._holdingObjectId)
			inventoryBag->addInventoryItem(inventoryItem, 0);
	}
}

} // End of namespace Illusions
