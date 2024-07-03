package products

import (
	"time"

	"gorm.io/gorm"
)

type ProductEntity struct {
	ID        uint           `gorm:"primaryKey"`
	Name      string         `gorm:"column:name"`
	Price     int            `gorm:"column:price"`
	ImageUrl  string         `gorm:"column:image_url"`
	Count     int            `gorm:"column:count"`
	Type      string         `gorm:"column:type"`
	CreatedAt time.Time      `gorm:"column:created_at"`
	UpdatedAt time.Time      `gorm:"column:updated_at"`
	DeletedAt gorm.DeletedAt `gorm:"column:deleted_at"`
}

func (ProductEntity) TableName() string {
	return "products"
}
