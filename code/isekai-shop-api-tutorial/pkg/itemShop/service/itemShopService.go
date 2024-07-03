package service

import (
	_itemShopModel "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/model"
)

type ItemShopService interface {
	Listing(itemFilter *_itemShopModel.ItemFilter) ([]*_itemShopModel.Item, error)
}
