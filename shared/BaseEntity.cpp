//in the name of ALLAH
//YA MAHDI

#include "BaseEntity.h"

quint64 BaseEntity::idCounter = 1;

BaseEntity::BaseEntity() {
    id = 0;
    createdAt = QDateTime::currentDateTime();
    updatedAt = QDateTime::currentDateTime();
}

BaseEntity::~BaseEntity() {
}

quint64 BaseEntity::getId() const {
    return id;
}

QDateTime BaseEntity::getCreatedAt() const {
    return createdAt;
}

QDateTime BaseEntity::getUpdatedAt() const {
    return updatedAt;
}

void BaseEntity::setId(quint64 newId) {
    id = newId;
}

void BaseEntity::assignNewId() {
    id = generateId();
}

void BaseEntity::touchUpdatedAt() {
    updatedAt = QDateTime::currentDateTime();
}

quint64 BaseEntity::generateId() {
    quint64 newId = idCounter;
    idCounter = idCounter + 1;
    return newId;
}
