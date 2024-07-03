package products

import (
	"context"
	"errors"
	"log"
)

type storage interface {
	CreateProduct(ctx context.Context, product ProductEntity) (*uint, error)
	GetProductByID(ctx context.Context, id uint) (*ProductEntity, error)
	UpdateProduct(ctx context.Context, product ProductEntity) error
	DeleteProduct(ctx context.Context, id uint) error
	ListProducts(ctx context.Context) (*[]ProductEntity, error)
}

type domain interface {
	ValidateNewProduct(ctx context.Context, product Product) error
}

type Usecase struct {
	storage storage
	domain  domain
}

func NewUsecase(s storage, d domain) *Usecase {
	return &Usecase{
		storage: s,
		domain:  d,
	}
}

func (u *Usecase) NewProduct(ctx context.Context, product Product) error {
	if err := u.domain.ValidateNewProduct(ctx, product); err != nil {
		if errors.Is(err, ErrProductNameExists) {
			return ErrProductNameExists
		}
		log.Printf("Error validating new product: %v", err)
		return err
	}

	productEntity := ProductEntity{
		Name:     product.Name,
		Price:    product.Price,
		ImageUrl: product.ImageUrl,
		Count:    product.Count,
		Type:     product.Type,
	}
	log.Printf("ProductEntity: %+v\n", productEntity)

	_, err := u.storage.CreateProduct(ctx, productEntity)
	return err
}

func (u *Usecase) ListProducts(ctx context.Context) (*[]ProductEntity, error) {
	products, err := u.storage.ListProducts(ctx)
	if err != nil {
		return nil, err
	}
	return products, nil
}

func (u *Usecase) DeleteProduct(ctx context.Context, id uint) error {
	return u.storage.DeleteProduct(ctx, id)
}

func (u *Usecase) UpdateProduct(ctx context.Context, product Product) error {
	if err := u.domain.ValidateNewProduct(ctx, product); err != nil {
		if errors.Is(err, ErrProductNameExists) {
			return ErrProductNameExists
		}
		log.Printf("Error validating updated product: %v", err)
		return err
	}

	productEntity := ProductEntity{
		ID:       product.ID,
		Name:     product.Name,
		Price:    product.Price,
		ImageUrl: product.ImageUrl,
		Count:    product.Count,
		Type:     product.Type,
	}

	if err := u.storage.UpdateProduct(ctx, productEntity); err != nil {
		log.Printf("Error updating product: %v", err)
		return err
	}
	return nil
}

func (u *Usecase) GetProductByID(ctx context.Context, id uint) (*Product, error) {
	productEntity, err := u.storage.GetProductByID(ctx, id)
	if err != nil {
		return nil, err
	}

	return &Product{
		ID:    productEntity.ID,
		Name:  productEntity.Name,
		Price: productEntity.Price,
		Count: productEntity.Count,
		Type:  productEntity.Type,
	}, nil
}
