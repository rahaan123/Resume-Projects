public interface AlertRepository extends JpaRepository<StockAlert, Long> {
    List<StockAlert> findByTriggeredFalse();
}
