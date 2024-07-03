package products

import (
	"gorm.io/gorm"
)

type Product struct {
	gorm.Model
	ID       uint
	Name     string
	Price    int
	Count    int
	ImageUrl string
	Type     string
}

type NewProductRequest struct {
	Name     string `json:"name"`
	Price    int    `json:"price"`
	Count    int    `json:"count"`
	ImageUrl string `json:"image_url"`
	Type     string `json:"type"`
}

type UpdateProductRequest struct {
	ID uint `json:"id" binding:"required"`
	NewProductRequest
}
