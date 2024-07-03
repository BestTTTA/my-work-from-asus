package service

import (
	_itemShopException "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/exception"
	_itemShopModel "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/model"
	_itemShopRepository "github.com/BestTTTA/isekai-shop-api-tutorial/pkg/itemShop/repository"
)

type itemShopServiceImpl struct {
	itemShopRepository _itemShopRepository.ItemShopRepository
}

func NewItemShopServiceImpl(itemShopRepository _itemShopRepository.ItemShopRepository) ItemShopService {
	return &itemShopServiceImpl{itemShopRepository}
}

func (s *itemShopServiceImpl) Listing(itemFilter *_itemShopModel.ItemFilter) ([]*_itemShopModel.Item, error) {
	itemList, err := s.itemShopRepository.Listing(itemFilter)
	if err != nil {
		return nil, &_itemShopException.ItemListing{}
	}

	itemModelList := make([]*_itemShopModel.Item, 0)
	for _, item := range itemList {
		itemModelList = append(itemModelList, item.ToItemModel())
	}

	return itemModelList, nil
}
