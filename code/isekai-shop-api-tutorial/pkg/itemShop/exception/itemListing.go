package exception


type ItemListing struct {

}

func (* ItemListing) Error() string {
	return "item listing failed"
}