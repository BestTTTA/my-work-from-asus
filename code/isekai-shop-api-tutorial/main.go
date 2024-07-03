package main

import (
	"github.com/BestTTTA/isekai-shop-api-tutorial/config"
	"github.com/BestTTTA/isekai-shop-api-tutorial/databases"
	"github.com/BestTTTA/isekai-shop-api-tutorial/server"
)

func main() {

	conf := config.ConfigGetting()
	db := databases.NewPostgresDatabase(conf.Database)
	server := server.NewEchoServer(conf, db.ConnectionGetting())

	server.Start()

}
