@Service
public class StockAlertService {

    @Autowired
    private AlertRepository repository;

    public void saveAlert(AlertRequest req) {
        StockAlert alert = new StockAlert();
        alert.setSymbol(req.getSymbol().toUpperCase());
        alert.setTargetPrice(req.getTargetPrice());
        repository.save(alert);
    }

    public List<StockAlert> getAllAlerts() {
        return repository.findAll();
    }

    public List<StockAlert> getUntriggeredAlerts() {
        return repository.findByTriggeredFalse();
    }

    public void markTriggered(StockAlert alert) {
        alert.setTriggered(true);
        repository.save(alert);
    }
}
