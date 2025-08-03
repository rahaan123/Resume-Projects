@Entity
public class StockAlert {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String symbol;
    private double targetPrice;
    private boolean triggered = false;

    // getters and setters
}
