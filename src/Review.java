

class Review implements Comparable<Review>{
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

	@Override
	public int compareTo(Review o) {
		// TODO Auto-generated method stub
		return (this == o) ? 0 : score == o.score ? 
				(time == o.time ? 
					(help.total == o.help.total ? 
							(help.votes == o.help.votes ? text.compareTo(o.text) : (help.votes < o.help.votes ? -1 : 1))
							: (help.total < o.help.total ? -1 : 1))
					: (time < o.time ? -1 : 1))
				: (score < o.score ? -1 : 1);
					
	}
	
}