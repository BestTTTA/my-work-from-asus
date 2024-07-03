package database

import (
	"fmt"
	"log"
	"os"

	"github.com/BestTTTA/go-crud-postgres/app/products"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
)

type GormConnection struct {
	user     string
	password string
	host     string
	name     string
	port     string
}

func NewGormDBWithDefault() *gorm.DB {
	dbConfig := GormConnection{
		host:     os.Getenv("DB_HOST"),
		port:     os.Getenv("DB_PORT"),
		password: os.Getenv("DB_PASSWORD"),
		user:     os.Getenv("DB_USER"),
		name:     os.Getenv("DB_NAME"),
	}

	dsn := fmt.Sprintf("host=%s port=%s user=%s password=%s dbname=%s sslmode=disable", dbConfig.host, dbConfig.port, dbConfig.user, dbConfig.password, dbConfig.name)
	return NewGormDB(dsn)
}

func NewGormDB(dsn string) *gorm.DB {
	db, err := gorm.Open(postgres.Open(dsn), &gorm.Config{})
	if err != nil {
		log.Panic(err)
	}

	// if err := db.AutoMigrate(
	// 	&products.ProductEntity{}); err != nil {
	// 	log.Fatalf("Failed to run migrations: %v", err)
	// }

	productMigration(db)

	return db
}

func productMigration(db *gorm.DB) error {
	if err := db.Migrator().CreateTable(&products.ProductEntity{}); err != nil {
		return fmt.Errorf("failed to create ProductEntity: %w", err)
	}
	return nil
}
