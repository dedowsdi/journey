/*
 * A prototype pattern is used in software development when the type of objects
 * to create is determined by a prototypical instance, which is cloned to
 * produce new objects. This pattern is used, for example, when the inherent
 * cost of creating a new object in the standard way (e.g., using the new
 * keyword) is prohibitively expensive for a given application.
 */

#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
using namespace std;

/** Record is the base Prototype */
class Record {
public:
  virtual ~Record() {}
  virtual void print() = 0;
  virtual unique_ptr<Record> clone() = 0;
};

/** CarRecord is a Concrete Prototype */
class CarRecord : public Record {
private:
  string m_carName;
  int m_ID;

public:
  CarRecord(string carName, int ID) : m_carName(carName), m_ID(ID) {}

  void print() override {
    cout << "Car Record" << endl
         << "Name  : " << m_carName << endl
         << "Number: " << m_ID << endl
         << endl;
  }

  unique_ptr<Record> clone() override {
    return std::unique_ptr<CarRecord>(new CarRecord(*this));
  }
};

/** BikeRecord is the Concrete Prototype */
class BikeRecord : public Record {
private:
  string m_bikeName;
  int m_ID;

public:
  BikeRecord(string bikeName, int ID) : m_bikeName(bikeName), m_ID(ID) {}

  void print() override {
    cout << "Bike Record" << endl
         << "Name  : " << m_bikeName << endl
         << "Number: " << m_ID << endl
         << endl;
  }

  unique_ptr<Record> clone() override {
    return std::unique_ptr<BikeRecord>(new BikeRecord(*this));
  };
};

/** Opaque record type, avoids exposing concrete implementations */
enum RecordType { CAR, BIKE };

/** RecordFactory is the client */
class RecordFactory {
private:
  unordered_map<RecordType, unique_ptr<Record>, hash<int>> m_records;

public:
  RecordFactory() {
    m_records[CAR] = std::unique_ptr<CarRecord>(new CarRecord("Ferrari", 5050));
    m_records[BIKE] =
      std::unique_ptr<BikeRecord>(new BikeRecord("Yamaha", 2525));
  }

  unique_ptr<Record> createRecord(RecordType recordType) {
    return m_records[recordType]->clone();
  }
};

int main() {
  RecordFactory recordFactory;

  auto record = recordFactory.createRecord(CAR);
  record->print();

  record = recordFactory.createRecord(BIKE);
  record->print();
}
