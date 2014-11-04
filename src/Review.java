

class Review {
	public final String summary;
	public final double score;
	public final int time;
	public final Reviewer reviewer;
	public final Helpfulness help;
	public final Product product;
	public final String text;
	
	private Review(String summary, double score, int time, Reviewer reviewer, Helpfulness help, Product product, String text){
		this.summary = summary;
		this.score = score;
		this.time = time;
		this.reviewer = reviewer;
		this.help = help;
		this.product = product;
		this.text = text;
	}
	
	public static Review build(String summary, double score, int time, Reviewer reviewer, Helpfulness help, Product product, String text){
		return new Review(summary, score, time, reviewer, help, product, text);
	}
	
}