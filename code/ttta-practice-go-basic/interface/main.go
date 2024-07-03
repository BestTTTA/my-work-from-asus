package main

import "fmt"

type (
	Database interface {
		Insert() error
		Update() error
	}

	RealDB struct{}
	MockDB struct{}
)

func CheckRealDB() Database{
	return &RealDB{}
}
func CheckMockDB() Database{
	return &MockDB{}
}

func (r *RealDB) Insert() error {
	fmt.Println("Real Insert")
	return nil
}
func (r *RealDB) Update() error {
	fmt.Println("Real Update")
	return nil
}

func (r *MockDB) Insert() error {
	fmt.Println("Mock Insert")
	return nil
}
func (r *MockDB) Update() error {
	fmt.Println("Mock Update")
	return nil
}

func InsertSomething(db Database) error{
	return db.Insert()
}

func UpdateSomething(db Database) error {
	return db.Update()
}

func main() {
	realdb := &RealDB{}
	mockdb := &MockDB{}

	InsertSomething(realdb)
	InsertSomething(mockdb)

	UpdateSomething(realdb)
	UpdateSomething(mockdb)
}

