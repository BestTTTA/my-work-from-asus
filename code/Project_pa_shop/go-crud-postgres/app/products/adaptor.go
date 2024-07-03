package products

import (
	"context"
	"errors"

	"gorm.io/gorm"
)

var ErrProductNameExists = errors.New("product name already exists")

type Adaptor struct {
	db *gorm.DB
}

func NewAdaptor(db *gorm.DB) *Adaptor {
	return &Adaptor{db: db}
}

func (a *Adaptor) ValidateNewProduct(ctx context.Context, product Product) error {
	// Check if the product name is empty
	if product.Name == "" {
		return errors.New("product name cannot be empty")
	}

	// Check for duplicate product names
	var existingProduct Product
	if err := a.db.WithContext(ctx).Where("name = ?", product.Name).First(&existingProduct).Error; err != nil {
		if !errors.Is(err, gorm.ErrRecordNotFound) {
			return err
		}
	} else {
		return ErrProductNameExists
	}

	// Add additional validations as needed.
	return nil
}
