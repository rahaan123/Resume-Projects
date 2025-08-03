@Component
public class PricePoller {

    @Autowired
    private StockAlertService service;

    @Scheduled(fixedRate = 60000) // Every minute
    public void pollPrices() {
        List<StockAlert> alerts = service.getUntriggeredAlerts();

        for (StockAlert alert : alerts) {
            double currentPrice = fetchPrice(alert.getSymbol());
            if (currentPrice <= alert.getTargetPrice()) {
                System.out.println("ALERT: " + alert.getSymbol() + " hit $" + currentPrice);
                service.markTriggered(alert);
            }
        }
    }

    private double fetchPrice(String symbol) {
        // Dummy price fetch - you can integrate real API here like Yahoo Finance
        return Math.random() * 1000;
    }
}
