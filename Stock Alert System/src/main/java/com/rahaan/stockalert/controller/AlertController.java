@RestController
@RequestMapping("/alerts")
public class AlertController {

    @Autowired
    private StockAlertService alertService;

    @PostMapping
    public ResponseEntity<String> createAlert(@RequestBody AlertRequest request) {
        alertService.saveAlert(request);
        return ResponseEntity.ok("Alert created.");
    }

    @GetMapping
    public List<StockAlert> getAlerts() {
        return alertService.getAllAlerts();
    }
}
